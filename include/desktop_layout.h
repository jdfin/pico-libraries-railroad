#pragma once

#include "sensor.h"
#include "sensor2.h"
#include "turnout.h"

// T0: straight to spur 1            S3
// T1: straight to spur 3           //
// S0: in house                    // Spur 3
// S1,S2,S3: at spur ends         /S
// S4: at uncoupler           T1 //D=============== S2
//                              //      Spur 2
//                             //
//               S4           /D
// S0 ==========UUUU===========S=================== S1
//                            T0        Spur 1

// kato track pieces
constexpr int r490_mm = 192; // 192.42

// desktop layout
constexpr int s0_s4_mm = 60 + 246 + 123 + 123 / 2;
constexpr int s4_s1_mm = 123 / 2 + 123 + 246 + 227 + 174 + 114;
constexpr int s4_s2_mm =
    123 / 2 + 123 + r490_mm + 60 + r490_mm + 227 + 94;
constexpr int s4_s3_mm = 123 / 2 + 123 + r490_mm + 60 + 246 + 246;

constexpr int turnout_max = 2;
extern Turnout turnout[turnout_max];

constexpr int sensor2_max = 4;
extern Sensor2 sensor2[sensor2_max];

constexpr int sensor_max = 1;
extern Sensor sensor[sensor_max];

Sensor2 &sensor_home();            // sensor in house
Sensor &sensor_unc();             // sensor at uncoupler
Sensor2 &sensor_spur(int spur_num); // sensor at end of spur

void line_turnout_0(int spur_num);
void line_turnout_1(int spur_num);

int unc_to_spur_mm(int spur_num);
