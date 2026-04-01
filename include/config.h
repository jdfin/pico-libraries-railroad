#pragma once

#include "hardware/uart.h"

//      USE PWM GPIO                     GPIO ADC PWM USE
//                   +-----| USB |-----+
//  (t) T0A P0A   D0 | 1            40 | VBUS_OUT
//  (t) T0B P0B   D1 | 2            39 | VSYS_IO
//               GND | 3            38 | GND
//  (t) T1A P1A   D2 | 4            37 | 3V3_EN
//  (t) T1B P1B   D3 | 5            36 | 3V3_OUT
//  (t) TP  P2A   D4 | 6            35 | AREF
//          P2B   D5 | 7            34 | D28  A2  P6A
//               GND | 8            33 | GND
//          P3A   D6 | 9            32 | D27  A1  P5B
//  (s) S0  P3B   D7 | 10           31 | D26  A0  P5A CS (dcc)
//          P4A   D8 | 11           30 | RUN
//  (s) S1  P4B   D9 | 12           29 | D22      P3A
//               GND | 13           28 | GND
//          P5A  D10 | 14           27 | D21      P2B
//  (s) S2  P5B  D11 | 15           26 | D20      P2A
//          P6A  D12 | 16           25 | D19      P1B SIG (dcc)
//  (s) S3  P6B  D13 | 17           24 | D18      P1A PWR (dcc)
//               GND | 18           23 | GND
//          P7A  D14 | 19           22 | D17      P0B RXD (dcc)
//  (s) S4  P7B  D15 | 20           21 | D16      P0A
//                   +-----------------+
//
// PWM usage
// Slice Channel Alts    Pin Use
// 0     A       D0/D16
// 0     B       D1/D17  *
// 1     A       D2/D18  D18 DCC power
// 1     B       D3/D19  D19 DCC signal
// 2     A       D4/D20  D4  Turnout pulse
// 2     B       D5/D21  *
// 3     A       D6/D22
// 3     B       D7      D7  Sensor (pwm)
// 4     A       D8
// 4     B       D9      D9  Sensor (pwm)
// 5     A       D10/D26
// 5     B       D11/D27 D11 Sensor (pwm)
// 6     A       D12/D28
// 6     B       D13     D13 Sensor (pwm)
// 7     A       D14
// 7     B       D15     D15 Sensor (dig)

constexpr int dcc_sig_gpio = 19;          // PH - pwm slice 1 channel B
constexpr int dcc_pwr_gpio = 18;          // EN - pwm slice 1 channel A
constexpr int dcc_adc_gpio = 26;          // CS (ADC0)
constexpr int dcc_rcom_gpio = 17;         // Railcom
uart_inst_t *const dcc_rcom_uart = uart0; // uart0

constexpr int dcc_dbg_rcom_read_gpio = -1;
constexpr int dcc_dbg_rcom_junk_gpio = -1;
constexpr int dcc_dbg_rcom_short_gpio = -1;
constexpr int dcc_dbg_bitstream_next_bit_gpio = -1;
constexpr int dcc_dbg_command_get_packet_gpio = -1;

constexpr int t0a_gpio = 0;
constexpr int t0b_gpio = 1;
constexpr int t1a_gpio = 2;
constexpr int t1b_gpio = 3;
constexpr int tp_gpio = 4; // pwm slice 2 channel A

constexpr int s0_gpio = 7;  // pwm sensor; slice 3 channel B
constexpr int s1_gpio = 9;  // pwm sensor; slice 4 channel B
constexpr int s2_gpio = 11; // pwm sensor; slice 5 channel B
constexpr int s3_gpio = 13; // pwm sensor; slice 6 channel B
constexpr int s4_gpio = 15; // digital sensor
