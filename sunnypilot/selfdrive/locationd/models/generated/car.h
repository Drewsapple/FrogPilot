#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_5268947421038592713);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8685912228915578263);
void car_H_mod_fun(double *state, double *out_4496494188713838935);
void car_f_fun(double *state, double dt, double *out_5982941645595593864);
void car_F_fun(double *state, double dt, double *out_2612312452143880537);
void car_h_25(double *state, double *unused, double *out_5743095908096626276);
void car_H_25(double *state, double *unused, double *out_4757061659083475444);
void car_h_24(double *state, double *unused, double *out_1600645818927796882);
void car_H_24(double *state, double *unused, double *out_2584412060077975878);
void car_h_30(double *state, double *unused, double *out_8841964689184423688);
void car_H_30(double *state, double *unused, double *out_7275394617590724071);
void car_h_26(double *state, double *unused, double *out_8396294644248070551);
void car_H_26(double *state, double *unused, double *out_1015558340209419220);
void car_h_27(double *state, double *unused, double *out_2326822618447556260);
void car_H_27(double *state, double *unused, double *out_8947755384934884328);
void car_h_29(double *state, double *unused, double *out_3840291421523578043);
void car_H_29(double *state, double *unused, double *out_7785625961905116255);
void car_h_28(double *state, double *unused, double *out_742256162125782635);
void car_H_28(double *state, double *unused, double *out_2703226944835585681);
void car_h_31(double *state, double *unused, double *out_1224219256584406218);
void car_H_31(double *state, double *unused, double *out_389350237976067744);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}