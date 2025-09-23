#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_7200996662305156998);
void live_err_fun(double *nom_x, double *delta_x, double *out_8934746366981719009);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_7054943301959371583);
void live_H_mod_fun(double *state, double *out_1277001528112563092);
void live_f_fun(double *state, double dt, double *out_1687708185263392323);
void live_F_fun(double *state, double dt, double *out_8730655152354205089);
void live_h_4(double *state, double *unused, double *out_9144165495913461103);
void live_H_4(double *state, double *unused, double *out_8275031715845290885);
void live_h_9(double *state, double *unused, double *out_9165247480760488457);
void live_H_9(double *state, double *unused, double *out_8033842069215700240);
void live_h_10(double *state, double *unused, double *out_9207538887712285807);
void live_H_10(double *state, double *unused, double *out_5312131839381710895);
void live_h_12(double *state, double *unused, double *out_7037707415024016137);
void live_H_12(double *state, double *unused, double *out_7653932690797697218);
void live_h_35(double *state, double *unused, double *out_862932496712363075);
void live_H_35(double *state, double *unused, double *out_4908369658472683509);
void live_h_32(double *state, double *unused, double *out_4420277559251492105);
void live_H_32(double *state, double *unused, double *out_6623509911542751751);
void live_h_13(double *state, double *unused, double *out_1115299477661242540);
void live_H_13(double *state, double *unused, double *out_2196413914031783836);
void live_h_14(double *state, double *unused, double *out_9165247480760488457);
void live_H_14(double *state, double *unused, double *out_8033842069215700240);
void live_h_33(double *state, double *unused, double *out_8753119431694184412);
void live_H_33(double *state, double *unused, double *out_1757812653833825905);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}