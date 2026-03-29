#pragma once

#include "sensor.h"
#include "turnout.h"

// T0: straight to Spur 1                 S7  S6
// T1: straight to Spur 3         ==============
// S2,S4,S6 50 mm from end       //       Spur 3
// S3,S5,S7 110 mm from end     //
// S1 at uncoupler             /S         S5  S4
// S0 in house             T1 //D===============
//                           //           Spur 2
//                          //
//  S0        S1           /D             S3  S2
// ==========UUUU===========S===================
//                         T0             Spur 1

// kato track pieces
constexpr int r490_mm = 192; // 192.42

// desktop layout
constexpr int s0_s1_mm = 246 + 123 + 123 / 2;
constexpr int s1_s3_mm = 123 / 2 + 123 + 246 + 227 + 174 + 114 - 110;
constexpr int s1_s5_mm =
    123 / 2 + 123 + r490_mm + 60 + r490_mm + 227 + 94 - 110;
constexpr int s1_s7_mm = 123 / 2 + 123 + r490_mm + 60 + 246 + 246 - 110;
constexpr int s3_s2_mm = 110 - 50;
constexpr int s5_s4_mm = 110 - 50;
constexpr int s7_s6_mm = 110 - 50;

constexpr int turnout_max = 2;
extern Turnout turnout[turnout_max];

constexpr int sensor_max = 8;
extern Sensor sensor[sensor_max];

Sensor &sensor_home();            // sensor in house
Sensor &sensor_unc();             // sensor at uncoupler
Sensor &sensor_50(int spur_num);  // sensor 50 mm from end of spur
Sensor &sensor_100(int spur_num); // sensor 100 mm from end of spur

void line_turnout_0(int spur_num);
void line_turnout_1(int spur_num);

int unc_to_spur_mm(int spur_num);
