#pragma once

#include "hardware/uart.h"
#include "hardware/i2c.h"

// 1: stacked boards
// 2: single-board
#define HW_VERSION 2

#if (HW_VERSION == 2)

// | MODE     | SIGNAL   | PWM | GPIO                   GPIO | ADC | PWM | SIGNAL     | MODE     |
// |          |          |     |     +-----| USB |-----+     |     |     |            |          |
// | SPI0_RX  | SPI_MISO | P0A |  D0 | 1            40 |     |     |     | VBUS_OUT   |          |
// | SPI0_CS  | SPI_CS   | P0B |  D1 | 2            39 |     |     |     | VSYS_IO    |          |
// |          | GND      |     |     | 3            38 |     |     |     | GND        |          |
// | SPI0_SCK | SPI_SCK  | P1A |  D2 | 4            37 |     |     |     | 3V3_EN     |          |
// | SPI0_TX  | SPI_MOSI | P1B |  D3 | 5            36 |     |     |     | 3V3_OUT    |          |
// | GPIO4    | SWITCH_1 | P2A |  D4 | 6            35 |     |     |     | AREF       |          |
// | PWM2_B   | SENSOR_4 | P2B |  D5 | 7            34 | D28 | A2  | P6A | TURNOUT_0A | GPIO28   |
// |          | GND      |     |     | 8            33 |     |     |     | GND        |          |
// | GPIO6    | SWITCH_0 | P3A |  D6 | 9            32 | D27 | A1  | P5B | TURNOUT_0B | GPIO27   |
// | PWM3_B   | SENSOR_3 | P3B |  D7 | 10           31 | D26 | A0  | P5A | DCC_CS     | A0       |
// |          |          | P4A |  D8 | 11           30 |     |     |     | RESET      |          |
// | PWM4_B   | SENSOR_2 | P4B |  D9 | 12           29 | D22 |     | P3A |            |          |
// |          | GND      |     |     | 13           28 |     |     |     | GND        |          |
// |          |          | P5A | D10 | 14           27 | D21 |     | P2B | TURNOUT_1A | GPIO21   |
// | PWM5_B   | SENSOR_1 | P5B | D11 | 15           26 | D20 |     | P2A | TURNOUT_1B | GPIO20   |
// | PIO      | WS2812B  | P6A | D12 | 16           25 | D19 |     | P1B | DCC_BIT    | PWM1_B   |
// | PWM6_B   | SENSOR_0 | P6B | D13 | 17           24 | D18 |     | P1A | DCC_CUT    | PWM1_A   |
// |          | GND      |     |     | 18           23 |     |     |     | GND        |          |
// | I2C1_SDA | I2C_SDA  | P7A | D14 | 19           22 | D17 |     | P0B | RC_DATA    | UART0_RX |
// | I2C1_SCL | I2C_SCL  | P7B | D15 | 20           21 | D16 |     | P0A | TURNOUT_P  | PWM0_A   |
//                                   +-----------------+

// PWM slice usage:
//   0 Turnouts (TURNOUT_P)
//   1 DCC      (DCC_BIT, DCC_CUT)
//   2 Sensor 4
//   3 Sensor 3
//   4 Sensor 2
//   5 Sensor 1
//   6 Sensor 0
//   7 (unused)

// DCC
constexpr int dcc_bit_gpio = 19;          // PH - pwm slice 1 channel B
constexpr int dcc_pwr_gpio = 18;          // EN - pwm slice 1 channel A
constexpr int dcc_adc_gpio = 26;          // CS (ADC0)
constexpr int dcc_rcom_gpio = 17;         // Railcom
uart_inst_t *const dcc_rcom_uart = uart0; // uart0

// Turnouts
constexpr int t0a_gpio = 28;
constexpr int t0b_gpio = 27;
constexpr int t1a_gpio = 21;
constexpr int t1b_gpio = 20;
constexpr int tp_gpio = 16; // pwm slice 0 channel A

// Sensors
constexpr int s0_gpio = 13; // pwm sensor, in house
constexpr int s1_gpio = 11; // pwm sensor, end of spur 1
constexpr int s2_gpio = 9;  // pwm sensor, end of spur 2
constexpr int s3_gpio = 7;  // pwm sensor, end of spur 3
constexpr int s4_gpio = 5;  // digital sensor, uncoupler

// Switches
constexpr int sw0_gpio = 6;
constexpr int sw1_gpio = 4;

// WS2812 LEDs
constexpr int ws2812_gpio = 12;

// Display
i2c_inst_t *const disp_i2c_dev = i2c1;
constexpr uint8_t disp_i2c_adrs = 0x3c;
constexpr int disp_sda_gpio = 14;
constexpr int disp_scl_gpio = 15;

#elif (HW_VERSION == 1)

// | MODE     | SIGNAL   | PWM | GPIO                   GPIO | ADC | PWM | SIGNAL     | MODE     |
// |          |          |     |     +-----| USB |-----+     |     |     |            |          |
// | SPI0_RX  | SPI_MISO | P0A |  D0 | 1            40 |     |     |     | VBUS_OUT   |          |
// | SPI0_CS  | SPI_CS   | P0B |  D1 | 2            39 |     |     |     | VSYS_IO    |          |
// |          | GND      |     |     | 3            38 |     |     |     | GND        |          |
// | SPI0_SCK | SPI_SCK  | P1A |  D2 | 4            37 |     |     |     | 3V3_EN     |          |
// | SPI0_TX  | SPI_MOSI | P1B |  D3 | 5            36 |     |     |     | 3V3_OUT    |          |
// | GPIO4    | SWITCH_0 | P2A |  D4 | 6            35 |     |     |     | AREF       |          |
// | PWM2_B   | SENSOR_0 | P2B |  D5 | 7            34 | D28 | A2  | P6A | TURNOUT_1B | GPIO28   |
// |          | GND      |     |     | 8            33 |     |     |     | GND        |          |
// | GPIO6    | SWITCH_1 | P3A |  D6 | 9            32 | D27 | A1  | P5B | TURNOUT_1A | GPIO27   |
// | PWM3_B   | SENSOR_1 | P3B |  D7 | 10           31 | D26 | A0  | P5A | DCC_CS     | A0       |
// | GPIO8    | LED_R    | P4A |  D8 | 11           30 |     |     |     | RESET      |          |
// | PWM4_B   | SENSOR_2 | P4B |  D9 | 12           29 | D22 |     | P3A | not used   |          |
// |          | GND      |     |     | 13           28 |     |     |     | GND        |          |
// | GPIO10   | LED_G    | P5A | D10 | 14           27 | D21 |     | P2B | TURNOUT_0B | GPIO21   |
// | PWM5_B   | SENSOR_3 | P5B | D11 | 15           26 | D20 |     | P2A | TURNOUT_0A | GPIO20   |
// | GPIO12   | LED_B    | P6A | D12 | 16           25 | D19 |     | P1B | DCC_BIT    | PWM1_B   |
// | GPIO13   | SENSOR_4 | P6B | D13 | 17           24 | D18 |     | P1A | DCC_PWR    | PWM1_A   |
// |          | GND      |     |     | 18           23 |     |     |     | GND        |          |
// | I2C1_SDA | I2C_SDA  | P7A | D14 | 19           22 | D17 |     | P0B | RC_DATA    | UART0_RX |
// | I2C1_SCL | I2C_SCL  | P7B | D15 | 20           21 | D16 |     | P0A | TURNOUT_P  | PWM0_A   |
//                                   +-----------------+

// PWM slice usage:
//   0 Turnouts (TURNOUT_P)
//   1 DCC      (DCC_BIT, DCC_PWR)
//   2 Sensor 0
//   3 Sensor 1
//   4 Sensor 2
//   5 Sensor 3
//   6 Sensor 4
//   7 (unused)

// DCC
constexpr int dcc_bit_gpio = 19;          // PH - pwm slice 1 channel B
constexpr int dcc_pwr_gpio = 18;          // EN - pwm slice 1 channel A
constexpr int dcc_adc_gpio = 26;          // CS (ADC0)
constexpr int dcc_rcom_gpio = 17;         // Railcom
uart_inst_t *const dcc_rcom_uart = uart0; // uart0

// Turnouts
constexpr int t0a_gpio = 20;
constexpr int t0b_gpio = 21;
constexpr int t1a_gpio = 27;
constexpr int t1b_gpio = 28;
constexpr int tp_gpio = 16; // pwm slice 0 channel A

// Sensors
constexpr int s0_gpio = 5;  // pwm sensor; slice 2 channel B
constexpr int s1_gpio = 7;  // pwm sensor; slice 3 channel B
constexpr int s2_gpio = 9;  // pwm sensor; slice 4 channel B
constexpr int s3_gpio = 11; // pwm sensor; slice 5 channel B
constexpr int s4_gpio = 13; // digital sensor

// Switches
constexpr int sw0_gpio = 4;
constexpr int sw1_gpio = 6;

// RGB LED
#define USE_WS2812 1
#if USE_WS2812
constexpr int ws2812_gpio = 12;
#else
constexpr int led_red_gpio = 8;
constexpr int led_green_gpio = 10;
constexpr int led_blue_gpio = 12;
#endif

#else

#error HW_VERSION!

#endif // HW_VERSION
