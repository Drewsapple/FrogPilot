import math

from cereal import car
from openpilot.common.numpy_fast import clip, interp
from openpilot.selfdrive.car import apply_meas_steer_torque_limits, apply_std_steer_angle_limits, common_fault_avoidance, create_gas_interceptor_command, make_can_msg, rate_limit
from openpilot.selfdrive.car.interfaces import CarControllerBase
from openpilot.selfdrive.car.toyota import toyotacan
from openpilot.selfdrive.car.toyota.values import CAR, STATIC_DSU_MSGS, NO_STOP_TIMER_CAR, TSS2_CAR, \
                                        MIN_ACC_SPEED, PEDAL_TRANSITION, CarControllerParams, ToyotaFlags, \
                                        UNSUPPORTED_DSU_CAR, STOP_AND_GO_CAR
from openpilot.selfdrive.controls.lib.drive_helpers import CRUISE_LONG_PRESS
from opendbc.can.packer import CANPacker

from openpilot.selfdrive.frogpilot.controls.lib.frogpilot_acceleration import get_max_allowed_accel

LongCtrlState = car.CarControl.Actuators.LongControlState
SteerControlType = car.CarParams.SteerControlType
VisualAlert = car.CarControl.HUDControl.VisualAlert

ACCELERATION_DUE_TO_GRAVITY = 9.81  # m/s^2

ACCEL_WINDUP_LIMIT = 0.5  # m/s^2 / frame

# LKA limits
# EPS faults if you apply torque while the steering rate is above 100 deg/s for too long
MAX_STEER_RATE = 100  # deg/s
MAX_STEER_RATE_FRAMES = 18  # tx control frames needed before torque can be cut

# EPS allows user torque above threshold for 50 frames before permanently faulting
MAX_USER_TORQUE = 500

# LTA limits
# EPS ignores commands above this angle and causes PCS to fault
MAX_LTA_ANGLE = 94.9461  # deg
MAX_LTA_DRIVER_TORQUE_ALLOWANCE = 150  # slightly above steering pressed allows some resistance when changing lanes

# Lock / unlock door commands - Credit goes to AlexandreSato!
LOCK_CMD = b"\x40\x05\x30\x11\x00\x80\x00\x00"
UNLOCK_CMD = b"\x40\x05\x30\x11\x00\x40\x00\x00"

PARK = car.CarState.GearShifter.park

class CarController(CarControllerBase):
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.params = CarControllerParams(self.CP)
    self.frame = 0
    self.last_steer = 0
    self.last_angle = 0
    self.alert_active = False
    self.last_standstill = False
    self.standstill_req = False
    self.steer_rate_counter = 0
    self.distance_button = 0

    self.pcm_accel_compensation = 0.0
    self.permit_braking = True

    self.packer = CANPacker(dbc_name)
    self.accel = 0

    # FrogPilot variables
    self.doors_locked = False
    self.reverse_cruise_active = False

    self.cruise_timer = 0
    self.previous_set_speed = 0

  def update(self, CC, CS, now_nanos, frogpilot_toggles):
    actuators = CC.actuators
    hud_control = CC.hudControl
    pcm_cancel_cmd = CC.cruiseControl.cancel
    lat_active = CC.latActive and abs(CS.out.steeringTorque) < MAX_USER_TORQUE

    # *** control msgs ***
    can_sends = []

    # *** steer torque ***
    new_steer = int(round(actuators.steer * self.params.STEER_MAX))
    apply_steer = apply_meas_steer_torque_limits(new_steer, self.last_steer, CS.out.steeringTorqueEps, self.params)

    # >100 degree/sec steering fault prevention
    self.steer_rate_counter, apply_steer_req = common_fault_avoidance(abs(CS.out.steeringRateDeg) >= MAX_STEER_RATE, lat_active,
                                                                      self.steer_rate_counter, MAX_STEER_RATE_FRAMES)

    if not lat_active:
      apply_steer = 0

    # *** steer angle ***
    if self.CP.steerControlType == SteerControlType.angle:
      # If using LTA control, disable LKA and set steering angle command
      apply_steer = 0
      apply_steer_req = False
      if self.frame % 2 == 0:
        # EPS uses the torque sensor angle to control with, offset to compensate
        apply_angle = actuators.steeringAngleDeg + CS.out.steeringAngleOffsetDeg

        # Angular rate limit based on speed
        apply_angle = apply_std_steer_angle_limits(apply_angle, self.last_angle, CS.out.vEgoRaw, self.params)

        if not lat_active:
          apply_angle = CS.out.steeringAngleDeg + CS.out.steeringAngleOffsetDeg

        self.last_angle = clip(apply_angle, -MAX_LTA_ANGLE, MAX_LTA_ANGLE)

    self.last_steer = apply_steer

    # toyota can trace shows STEERING_LKA at 42Hz, with counter adding alternatively 1 and 2;
    # sending it at 100Hz seem to allow a higher rate limit, as the rate limit seems imposed
    # on consecutive messages
    can_sends.append(toyotacan.create_steer_command(self.packer, apply_steer, apply_steer_req))

    # STEERING_LTA does not seem to allow more rate by sending faster, and may wind up easier
    if self.frame % 2 == 0 and self.CP.carFingerprint in TSS2_CAR:
      lta_active = lat_active and self.CP.steerControlType == SteerControlType.angle
      # cut steering torque with TORQUE_WIND_DOWN when either EPS torque or driver torque is above
      # the threshold, to limit max lateral acceleration and for driver torque blending respectively.
      full_torque_condition = (abs(CS.out.steeringTorqueEps) < self.params.STEER_MAX and
                               abs(CS.out.steeringTorque) < MAX_LTA_DRIVER_TORQUE_ALLOWANCE)

      # TORQUE_WIND_DOWN at 0 ramps down torque at roughly the max down rate of 1500 units/sec
      torque_wind_down = 100 if lta_active and full_torque_condition else 0
      can_sends.append(toyotacan.create_lta_steer_command(self.packer, self.CP.steerControlType, self.last_angle,
                                                          lta_active, self.frame // 2, torque_wind_down))

    # *** gas and brake ***
    if self.CP.enableGasInterceptor and CC.longActive and self.CP.carFingerprint not in STOP_AND_GO_CAR:
      MAX_INTERCEPTOR_GAS = 0.5
      # RAV4 has very sensitive gas pedal
      if self.CP.carFingerprint == CAR.TOYOTA_RAV4:
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.15, 0.3, 0.0])
      elif self.CP.carFingerprint == CAR.TOYOTA_COROLLA:
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.3, 0.4, 0.0])
      else:
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.4, 0.5, 0.0])
      # offset for creep and windbrake
      pedal_offset = interp(CS.out.vEgo, [0.0, 2.3, MIN_ACC_SPEED + PEDAL_TRANSITION], [-.4, 0.0, 0.2])
      pedal_command = PEDAL_SCALE * (actuators.accel + pedal_offset)
      interceptor_gas_cmd = clip(pedal_command, 0., MAX_INTERCEPTOR_GAS)
    elif self.CP.enableGasInterceptor and CC.longActive and self.CP.carFingerprint in STOP_AND_GO_CAR and actuators.accel > 0.0:
      interceptor_gas_cmd = 0.12 if CS.out.standstill else 0.
    else:
      interceptor_gas_cmd = 0.

    # For cars where we allow a higher max acceleration of 2.0 m/s^2, compensate for PCM request overshoot and imprecise braking
    if self.CP.flags & ToyotaFlags.RAISED_ACCEL_LIMIT and CC.longActive and not CS.out.cruiseState.standstill:
      # calculate amount of acceleration PCM should apply to reach target, given pitch
      if len(CC.orientationNED) == 3:
        accel_due_to_pitch = math.sin(CC.orientationNED[1]) * ACCELERATION_DUE_TO_GRAVITY
      else:
        accel_due_to_pitch = 0.0

      net_acceleration_request = actuators.accel + accel_due_to_pitch

      # let PCM handle stopping for now
      pcm_accel_compensation = 0.0
      if actuators.longControlState != LongCtrlState.stopping:
        pcm_accel_compensation = 2.0 * (CS.pcm_accel_net - net_acceleration_request)

      # prevent compensation windup
      pcm_accel_compensation = clip(pcm_accel_compensation, actuators.accel - self.params.ACCEL_MAX,
                                    actuators.accel - self.params.ACCEL_MIN)

      self.pcm_accel_compensation = rate_limit(pcm_accel_compensation, self.pcm_accel_compensation, -0.01, 0.01)
      pcm_accel_cmd = actuators.accel - self.pcm_accel_compensation

      # Along with rate limiting positive jerk below, this greatly improves gas response time
      # Consider the net acceleration request that the PCM should be applying (pitch included)
      if net_acceleration_request < 0.1:
        self.permit_braking = True
      elif net_acceleration_request > 0.2:
        self.permit_braking = False
    else:
      self.pcm_accel_compensation = 0.0
      pcm_accel_cmd = actuators.accel
      self.permit_braking = True

    if frogpilot_toggles.sport_plus:
      pcm_accel_cmd = clip(pcm_accel_cmd, self.params.ACCEL_MIN, min(frogpilot_toggles.max_desired_acceleration, get_max_allowed_accel(CS.out.vEgo)))
    else:
      pcm_accel_cmd = clip(pcm_accel_cmd, self.params.ACCEL_MIN, min(frogpilot_toggles.max_desired_acceleration, self.params.ACCEL_MAX))

    # on entering standstill, send standstill request
    if CS.out.standstill and not self.last_standstill and (self.CP.carFingerprint not in NO_STOP_TIMER_CAR or self.CP.enableGasInterceptor):
      self.standstill_req = True
    if CS.pcm_acc_status != 8 or frogpilot_toggles.sng_hack:
      # pcm entered standstill or it's disabled
      self.standstill_req = False

    self.last_standstill = CS.out.standstill

    # handle UI messages
    fcw_alert = hud_control.visualAlert == VisualAlert.fcw
    steer_alert = hud_control.visualAlert in (VisualAlert.steerRequired, VisualAlert.ldw)

    # we can spam can to cancel the system even if we are using lat only control
    if (self.frame % 3 == 0 and self.CP.openpilotLongitudinalControl) or pcm_cancel_cmd:
      lead = hud_control.leadVisible or CS.out.vEgo < 12.  # at low speed we always assume the lead is present so ACC can be engaged

      # Press distance button until we are at the correct bar length. Only change while enabled to avoid skipping startup popup
      if self.frame % 6 == 0 and self.CP.openpilotLongitudinalControl:
        desired_distance = 4 - hud_control.leadDistanceBars
        if CS.out.cruiseState.enabled and CS.pcm_follow_distance != desired_distance:
          self.distance_button = not self.distance_button
        else:
          self.distance_button = 0

      # Lexus IS uses a different cancellation message
      if pcm_cancel_cmd and self.CP.carFingerprint in UNSUPPORTED_DSU_CAR:
        can_sends.append(toyotacan.create_acc_cancel_command(self.packer))
      elif self.CP.openpilotLongitudinalControl:
        # internal PCM gas command can get stuck unwinding from negative accel so we apply a generous rate limit
        pcm_accel_cmd = min(pcm_accel_cmd, self.accel + ACCEL_WINDUP_LIMIT) if CC.longActive else 0.0

        can_sends.append(toyotacan.create_accel_command(self.packer, pcm_accel_cmd, pcm_cancel_cmd, self.permit_braking, self.standstill_req, lead,
                                                        CS.acc_type, fcw_alert, self.distance_button, self.reverse_cruise_active))
        self.accel = pcm_accel_cmd
      else:
        can_sends.append(toyotacan.create_accel_command(self.packer, 0, pcm_cancel_cmd, True, False, lead, CS.acc_type, False, self.distance_button, self.reverse_cruise_active))

    if self.frame % 2 == 0 and self.CP.enableGasInterceptor and self.CP.openpilotLongitudinalControl:
      # send exactly zero if gas cmd is zero. Interceptor will send the max between read value and gas cmd.
      # This prevents unexpected pedal range rescaling
      can_sends.append(create_gas_interceptor_command(self.packer, interceptor_gas_cmd, self.frame // 2))

    # *** hud ui ***
    if self.CP.carFingerprint != CAR.TOYOTA_PRIUS_V:
      # ui mesg is at 1Hz but we send asap if:
      # - there is something to display
      # - there is something to stop displaying
      send_ui = False
      if ((fcw_alert or steer_alert) and not self.alert_active) or \
         (not (fcw_alert or steer_alert) and self.alert_active):
        send_ui = True
        self.alert_active = not self.alert_active
      elif pcm_cancel_cmd:
        # forcing the pcm to disengage causes a bad fault sound so play a good sound instead
        send_ui = True

      if self.frame % 20 == 0 or send_ui:
        can_sends.append(toyotacan.create_ui_command(self.packer, steer_alert, pcm_cancel_cmd, hud_control.leftLaneVisible,
                                                     hud_control.rightLaneVisible, hud_control.leftLaneDepart,
                                                     hud_control.rightLaneDepart, CC.enabled, CS.lkas_hud, lat_active))

      if (self.frame % 100 == 0 or send_ui) and (self.CP.enableDsu or self.CP.flags & ToyotaFlags.DISABLE_RADAR.value):
        can_sends.append(toyotacan.create_fcw_command(self.packer, fcw_alert))

    # *** static msgs ***
    for addr, cars, bus, fr_step, vl in STATIC_DSU_MSGS:
      if self.frame % fr_step == 0 and self.CP.enableDsu and self.CP.carFingerprint in cars:
        can_sends.append(make_can_msg(addr, vl, bus))

    # keep radar disabled
    if self.frame % 20 == 0 and self.CP.flags & ToyotaFlags.DISABLE_RADAR.value:
      can_sends.append([0x750, 0, b"\x0F\x02\x3E\x00\x00\x00\x00\x00", 0])

    new_actuators = actuators.as_builder()
    new_actuators.steer = apply_steer / self.params.STEER_MAX
    new_actuators.steerOutputCan = apply_steer
    new_actuators.steeringAngleDeg = self.last_angle
    new_actuators.accel = self.accel

    # FrogPilot Toyota carcontroller functions
    if False: #self.previous_set_speed != CS.out.cruiseState.speedCluster:
      self.cruise_timer = CRUISE_LONG_PRESS
    elif self.cruise_timer > 0:
      self.cruise_timer -= 1
    else:
      self.previous_set_speed = CS.out.cruiseState.speedCluster

    # Lock doors when in drive / unlock doors when in park
    if not self.doors_locked and CS.out.gearShifter != PARK:
      if frogpilot_toggles.lock_doors:
        can_sends.append(make_can_msg(0x750, LOCK_CMD, 0))
      self.doors_locked = True
    elif self.doors_locked and CS.out.gearShifter == PARK:
      if frogpilot_toggles.unlock_doors:
        can_sends.append(make_can_msg(0x750, UNLOCK_CMD, 0))
      self.doors_locked = False

    self.reverse_cruise_active = frogpilot_toggles.reverse_cruise_increase
    self.reverse_cruise_active &= self.cruise_timer <= 0

    self.frame += 1
    return new_actuators, can_sends
