#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_5268947421038592713) {
   out_5268947421038592713[0] = delta_x[0] + nom_x[0];
   out_5268947421038592713[1] = delta_x[1] + nom_x[1];
   out_5268947421038592713[2] = delta_x[2] + nom_x[2];
   out_5268947421038592713[3] = delta_x[3] + nom_x[3];
   out_5268947421038592713[4] = delta_x[4] + nom_x[4];
   out_5268947421038592713[5] = delta_x[5] + nom_x[5];
   out_5268947421038592713[6] = delta_x[6] + nom_x[6];
   out_5268947421038592713[7] = delta_x[7] + nom_x[7];
   out_5268947421038592713[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8685912228915578263) {
   out_8685912228915578263[0] = -nom_x[0] + true_x[0];
   out_8685912228915578263[1] = -nom_x[1] + true_x[1];
   out_8685912228915578263[2] = -nom_x[2] + true_x[2];
   out_8685912228915578263[3] = -nom_x[3] + true_x[3];
   out_8685912228915578263[4] = -nom_x[4] + true_x[4];
   out_8685912228915578263[5] = -nom_x[5] + true_x[5];
   out_8685912228915578263[6] = -nom_x[6] + true_x[6];
   out_8685912228915578263[7] = -nom_x[7] + true_x[7];
   out_8685912228915578263[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_4496494188713838935) {
   out_4496494188713838935[0] = 1.0;
   out_4496494188713838935[1] = 0.0;
   out_4496494188713838935[2] = 0.0;
   out_4496494188713838935[3] = 0.0;
   out_4496494188713838935[4] = 0.0;
   out_4496494188713838935[5] = 0.0;
   out_4496494188713838935[6] = 0.0;
   out_4496494188713838935[7] = 0.0;
   out_4496494188713838935[8] = 0.0;
   out_4496494188713838935[9] = 0.0;
   out_4496494188713838935[10] = 1.0;
   out_4496494188713838935[11] = 0.0;
   out_4496494188713838935[12] = 0.0;
   out_4496494188713838935[13] = 0.0;
   out_4496494188713838935[14] = 0.0;
   out_4496494188713838935[15] = 0.0;
   out_4496494188713838935[16] = 0.0;
   out_4496494188713838935[17] = 0.0;
   out_4496494188713838935[18] = 0.0;
   out_4496494188713838935[19] = 0.0;
   out_4496494188713838935[20] = 1.0;
   out_4496494188713838935[21] = 0.0;
   out_4496494188713838935[22] = 0.0;
   out_4496494188713838935[23] = 0.0;
   out_4496494188713838935[24] = 0.0;
   out_4496494188713838935[25] = 0.0;
   out_4496494188713838935[26] = 0.0;
   out_4496494188713838935[27] = 0.0;
   out_4496494188713838935[28] = 0.0;
   out_4496494188713838935[29] = 0.0;
   out_4496494188713838935[30] = 1.0;
   out_4496494188713838935[31] = 0.0;
   out_4496494188713838935[32] = 0.0;
   out_4496494188713838935[33] = 0.0;
   out_4496494188713838935[34] = 0.0;
   out_4496494188713838935[35] = 0.0;
   out_4496494188713838935[36] = 0.0;
   out_4496494188713838935[37] = 0.0;
   out_4496494188713838935[38] = 0.0;
   out_4496494188713838935[39] = 0.0;
   out_4496494188713838935[40] = 1.0;
   out_4496494188713838935[41] = 0.0;
   out_4496494188713838935[42] = 0.0;
   out_4496494188713838935[43] = 0.0;
   out_4496494188713838935[44] = 0.0;
   out_4496494188713838935[45] = 0.0;
   out_4496494188713838935[46] = 0.0;
   out_4496494188713838935[47] = 0.0;
   out_4496494188713838935[48] = 0.0;
   out_4496494188713838935[49] = 0.0;
   out_4496494188713838935[50] = 1.0;
   out_4496494188713838935[51] = 0.0;
   out_4496494188713838935[52] = 0.0;
   out_4496494188713838935[53] = 0.0;
   out_4496494188713838935[54] = 0.0;
   out_4496494188713838935[55] = 0.0;
   out_4496494188713838935[56] = 0.0;
   out_4496494188713838935[57] = 0.0;
   out_4496494188713838935[58] = 0.0;
   out_4496494188713838935[59] = 0.0;
   out_4496494188713838935[60] = 1.0;
   out_4496494188713838935[61] = 0.0;
   out_4496494188713838935[62] = 0.0;
   out_4496494188713838935[63] = 0.0;
   out_4496494188713838935[64] = 0.0;
   out_4496494188713838935[65] = 0.0;
   out_4496494188713838935[66] = 0.0;
   out_4496494188713838935[67] = 0.0;
   out_4496494188713838935[68] = 0.0;
   out_4496494188713838935[69] = 0.0;
   out_4496494188713838935[70] = 1.0;
   out_4496494188713838935[71] = 0.0;
   out_4496494188713838935[72] = 0.0;
   out_4496494188713838935[73] = 0.0;
   out_4496494188713838935[74] = 0.0;
   out_4496494188713838935[75] = 0.0;
   out_4496494188713838935[76] = 0.0;
   out_4496494188713838935[77] = 0.0;
   out_4496494188713838935[78] = 0.0;
   out_4496494188713838935[79] = 0.0;
   out_4496494188713838935[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_5982941645595593864) {
   out_5982941645595593864[0] = state[0];
   out_5982941645595593864[1] = state[1];
   out_5982941645595593864[2] = state[2];
   out_5982941645595593864[3] = state[3];
   out_5982941645595593864[4] = state[4];
   out_5982941645595593864[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_5982941645595593864[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_5982941645595593864[7] = state[7];
   out_5982941645595593864[8] = state[8];
}
void F_fun(double *state, double dt, double *out_2612312452143880537) {
   out_2612312452143880537[0] = 1;
   out_2612312452143880537[1] = 0;
   out_2612312452143880537[2] = 0;
   out_2612312452143880537[3] = 0;
   out_2612312452143880537[4] = 0;
   out_2612312452143880537[5] = 0;
   out_2612312452143880537[6] = 0;
   out_2612312452143880537[7] = 0;
   out_2612312452143880537[8] = 0;
   out_2612312452143880537[9] = 0;
   out_2612312452143880537[10] = 1;
   out_2612312452143880537[11] = 0;
   out_2612312452143880537[12] = 0;
   out_2612312452143880537[13] = 0;
   out_2612312452143880537[14] = 0;
   out_2612312452143880537[15] = 0;
   out_2612312452143880537[16] = 0;
   out_2612312452143880537[17] = 0;
   out_2612312452143880537[18] = 0;
   out_2612312452143880537[19] = 0;
   out_2612312452143880537[20] = 1;
   out_2612312452143880537[21] = 0;
   out_2612312452143880537[22] = 0;
   out_2612312452143880537[23] = 0;
   out_2612312452143880537[24] = 0;
   out_2612312452143880537[25] = 0;
   out_2612312452143880537[26] = 0;
   out_2612312452143880537[27] = 0;
   out_2612312452143880537[28] = 0;
   out_2612312452143880537[29] = 0;
   out_2612312452143880537[30] = 1;
   out_2612312452143880537[31] = 0;
   out_2612312452143880537[32] = 0;
   out_2612312452143880537[33] = 0;
   out_2612312452143880537[34] = 0;
   out_2612312452143880537[35] = 0;
   out_2612312452143880537[36] = 0;
   out_2612312452143880537[37] = 0;
   out_2612312452143880537[38] = 0;
   out_2612312452143880537[39] = 0;
   out_2612312452143880537[40] = 1;
   out_2612312452143880537[41] = 0;
   out_2612312452143880537[42] = 0;
   out_2612312452143880537[43] = 0;
   out_2612312452143880537[44] = 0;
   out_2612312452143880537[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_2612312452143880537[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_2612312452143880537[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2612312452143880537[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2612312452143880537[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_2612312452143880537[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_2612312452143880537[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_2612312452143880537[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_2612312452143880537[53] = -9.8100000000000005*dt;
   out_2612312452143880537[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_2612312452143880537[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_2612312452143880537[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2612312452143880537[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2612312452143880537[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_2612312452143880537[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_2612312452143880537[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_2612312452143880537[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2612312452143880537[62] = 0;
   out_2612312452143880537[63] = 0;
   out_2612312452143880537[64] = 0;
   out_2612312452143880537[65] = 0;
   out_2612312452143880537[66] = 0;
   out_2612312452143880537[67] = 0;
   out_2612312452143880537[68] = 0;
   out_2612312452143880537[69] = 0;
   out_2612312452143880537[70] = 1;
   out_2612312452143880537[71] = 0;
   out_2612312452143880537[72] = 0;
   out_2612312452143880537[73] = 0;
   out_2612312452143880537[74] = 0;
   out_2612312452143880537[75] = 0;
   out_2612312452143880537[76] = 0;
   out_2612312452143880537[77] = 0;
   out_2612312452143880537[78] = 0;
   out_2612312452143880537[79] = 0;
   out_2612312452143880537[80] = 1;
}
void h_25(double *state, double *unused, double *out_5743095908096626276) {
   out_5743095908096626276[0] = state[6];
}
void H_25(double *state, double *unused, double *out_4757061659083475444) {
   out_4757061659083475444[0] = 0;
   out_4757061659083475444[1] = 0;
   out_4757061659083475444[2] = 0;
   out_4757061659083475444[3] = 0;
   out_4757061659083475444[4] = 0;
   out_4757061659083475444[5] = 0;
   out_4757061659083475444[6] = 1;
   out_4757061659083475444[7] = 0;
   out_4757061659083475444[8] = 0;
}
void h_24(double *state, double *unused, double *out_1600645818927796882) {
   out_1600645818927796882[0] = state[4];
   out_1600645818927796882[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2584412060077975878) {
   out_2584412060077975878[0] = 0;
   out_2584412060077975878[1] = 0;
   out_2584412060077975878[2] = 0;
   out_2584412060077975878[3] = 0;
   out_2584412060077975878[4] = 1;
   out_2584412060077975878[5] = 0;
   out_2584412060077975878[6] = 0;
   out_2584412060077975878[7] = 0;
   out_2584412060077975878[8] = 0;
   out_2584412060077975878[9] = 0;
   out_2584412060077975878[10] = 0;
   out_2584412060077975878[11] = 0;
   out_2584412060077975878[12] = 0;
   out_2584412060077975878[13] = 0;
   out_2584412060077975878[14] = 1;
   out_2584412060077975878[15] = 0;
   out_2584412060077975878[16] = 0;
   out_2584412060077975878[17] = 0;
}
void h_30(double *state, double *unused, double *out_8841964689184423688) {
   out_8841964689184423688[0] = state[4];
}
void H_30(double *state, double *unused, double *out_7275394617590724071) {
   out_7275394617590724071[0] = 0;
   out_7275394617590724071[1] = 0;
   out_7275394617590724071[2] = 0;
   out_7275394617590724071[3] = 0;
   out_7275394617590724071[4] = 1;
   out_7275394617590724071[5] = 0;
   out_7275394617590724071[6] = 0;
   out_7275394617590724071[7] = 0;
   out_7275394617590724071[8] = 0;
}
void h_26(double *state, double *unused, double *out_8396294644248070551) {
   out_8396294644248070551[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1015558340209419220) {
   out_1015558340209419220[0] = 0;
   out_1015558340209419220[1] = 0;
   out_1015558340209419220[2] = 0;
   out_1015558340209419220[3] = 0;
   out_1015558340209419220[4] = 0;
   out_1015558340209419220[5] = 0;
   out_1015558340209419220[6] = 0;
   out_1015558340209419220[7] = 1;
   out_1015558340209419220[8] = 0;
}
void h_27(double *state, double *unused, double *out_2326822618447556260) {
   out_2326822618447556260[0] = state[3];
}
void H_27(double *state, double *unused, double *out_8947755384934884328) {
   out_8947755384934884328[0] = 0;
   out_8947755384934884328[1] = 0;
   out_8947755384934884328[2] = 0;
   out_8947755384934884328[3] = 1;
   out_8947755384934884328[4] = 0;
   out_8947755384934884328[5] = 0;
   out_8947755384934884328[6] = 0;
   out_8947755384934884328[7] = 0;
   out_8947755384934884328[8] = 0;
}
void h_29(double *state, double *unused, double *out_3840291421523578043) {
   out_3840291421523578043[0] = state[1];
}
void H_29(double *state, double *unused, double *out_7785625961905116255) {
   out_7785625961905116255[0] = 0;
   out_7785625961905116255[1] = 1;
   out_7785625961905116255[2] = 0;
   out_7785625961905116255[3] = 0;
   out_7785625961905116255[4] = 0;
   out_7785625961905116255[5] = 0;
   out_7785625961905116255[6] = 0;
   out_7785625961905116255[7] = 0;
   out_7785625961905116255[8] = 0;
}
void h_28(double *state, double *unused, double *out_742256162125782635) {
   out_742256162125782635[0] = state[0];
}
void H_28(double *state, double *unused, double *out_2703226944835585681) {
   out_2703226944835585681[0] = 1;
   out_2703226944835585681[1] = 0;
   out_2703226944835585681[2] = 0;
   out_2703226944835585681[3] = 0;
   out_2703226944835585681[4] = 0;
   out_2703226944835585681[5] = 0;
   out_2703226944835585681[6] = 0;
   out_2703226944835585681[7] = 0;
   out_2703226944835585681[8] = 0;
}
void h_31(double *state, double *unused, double *out_1224219256584406218) {
   out_1224219256584406218[0] = state[8];
}
void H_31(double *state, double *unused, double *out_389350237976067744) {
   out_389350237976067744[0] = 0;
   out_389350237976067744[1] = 0;
   out_389350237976067744[2] = 0;
   out_389350237976067744[3] = 0;
   out_389350237976067744[4] = 0;
   out_389350237976067744[5] = 0;
   out_389350237976067744[6] = 0;
   out_389350237976067744[7] = 0;
   out_389350237976067744[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_5268947421038592713) {
  err_fun(nom_x, delta_x, out_5268947421038592713);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8685912228915578263) {
  inv_err_fun(nom_x, true_x, out_8685912228915578263);
}
void car_H_mod_fun(double *state, double *out_4496494188713838935) {
  H_mod_fun(state, out_4496494188713838935);
}
void car_f_fun(double *state, double dt, double *out_5982941645595593864) {
  f_fun(state,  dt, out_5982941645595593864);
}
void car_F_fun(double *state, double dt, double *out_2612312452143880537) {
  F_fun(state,  dt, out_2612312452143880537);
}
void car_h_25(double *state, double *unused, double *out_5743095908096626276) {
  h_25(state, unused, out_5743095908096626276);
}
void car_H_25(double *state, double *unused, double *out_4757061659083475444) {
  H_25(state, unused, out_4757061659083475444);
}
void car_h_24(double *state, double *unused, double *out_1600645818927796882) {
  h_24(state, unused, out_1600645818927796882);
}
void car_H_24(double *state, double *unused, double *out_2584412060077975878) {
  H_24(state, unused, out_2584412060077975878);
}
void car_h_30(double *state, double *unused, double *out_8841964689184423688) {
  h_30(state, unused, out_8841964689184423688);
}
void car_H_30(double *state, double *unused, double *out_7275394617590724071) {
  H_30(state, unused, out_7275394617590724071);
}
void car_h_26(double *state, double *unused, double *out_8396294644248070551) {
  h_26(state, unused, out_8396294644248070551);
}
void car_H_26(double *state, double *unused, double *out_1015558340209419220) {
  H_26(state, unused, out_1015558340209419220);
}
void car_h_27(double *state, double *unused, double *out_2326822618447556260) {
  h_27(state, unused, out_2326822618447556260);
}
void car_H_27(double *state, double *unused, double *out_8947755384934884328) {
  H_27(state, unused, out_8947755384934884328);
}
void car_h_29(double *state, double *unused, double *out_3840291421523578043) {
  h_29(state, unused, out_3840291421523578043);
}
void car_H_29(double *state, double *unused, double *out_7785625961905116255) {
  H_29(state, unused, out_7785625961905116255);
}
void car_h_28(double *state, double *unused, double *out_742256162125782635) {
  h_28(state, unused, out_742256162125782635);
}
void car_H_28(double *state, double *unused, double *out_2703226944835585681) {
  H_28(state, unused, out_2703226944835585681);
}
void car_h_31(double *state, double *unused, double *out_1224219256584406218) {
  h_31(state, unused, out_1224219256584406218);
}
void car_H_31(double *state, double *unused, double *out_389350237976067744) {
  H_31(state, unused, out_389350237976067744);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
