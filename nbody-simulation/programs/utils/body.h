//
// Created by cjdcoy on 08/11/19.
//

#ifndef BODY_H_
#define BODY_H_

#include <random>
#include <algorithm>
#include "x11-helpers.h"

#define G_CONST 0.01
#define POS_LOW_BOUNDS 0
#define VEL_LOW_BOUNDS 0
#define VEL_HIGH_BOUNDS 1
#define MASS_LOW_BOUNDS 0
#define MASS_HIGH_BOUNDS 10
#define ACCEL_DEFAULT 0
#define SEED 42
#define NB_BODIES 200

typedef struct body_s {
    double x_pos;
    double y_pos;
    double x_vel;
    double y_vel;
    double x_accel;
    double y_accel;
    double mass;

} body_t;

void generate_bodies(std::vector<body_t> &bodies, int nb_body);

#endif //NBODY_PTHREAD_GENERATE_H
