
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h" // sleep_ms
//
#include "railroad/config.h"
#include "railroad/sensor2.h"


//  PWM            +-----| USB |-----+            PWM
//   0A         D0 | 1            40 | VBUS_OUT
//   0B         D1 | 2            39 | VSYS_IO
//             GND | 3            38 | GND
//   1A         D2 | 4            37 | 3V3_EN
//   1B         D3 | 5            36 | 3V3_OUT
//   2A         D4 | 6            35 | AREF
//   2B   S0    D5 | 7            34 | A2/D28     6A
//             GND | 8            33 | GND
//   3A         D6 | 9            32 | A1/D27     5B
//   3B   S1    D7 | 10           31 | A0/D26     5A
//   4A         D8 | 11           30 | RUN
//   4B   S2    D9 | 12           29 | D22        3A
//             GND | 13           28 | GND
//   5A        D10 | 14           27 | D21        2B
//   5B   S3   D11 | 15           26 | D20        2A
//   6A        D12 | 16           25 | D19        1B
//   6B   S4   D13 | 17           24 | D18        1A
//             GND | 18           23 | GND
//   7A        D14 | 19           22 | D17        0B
//   7B        D15 | 20           21 | D16        0A
//                 +-----------------+

// Each sensor has to be a PWM channel B.

// pwm sensors only (not the uncoupler S4)

static Sensor2 sensor[] = {
    Sensor2(s0_gpio),
    Sensor2(s1_gpio),
    Sensor2(s2_gpio),
    Sensor2(s3_gpio),
};

static constexpr int sensor_max = sizeof(sensor) / sizeof(sensor[0]);


int main()
{
    stdio_init_all();

#if 1
    while (!stdio_usb_connected())
        tight_loop_contents();
#endif

    sleep_ms(100);

    printf("\n");
    printf("sensor2_test\n");
    printf("\n");

    for (int i = 0; i < sensor_max; i++)
        sensor[i].init();

    int last_mm[sensor_max];

    for (int i = 0; i < sensor_max; i++)
        last_mm[i] = INT_MAX;

    while (true) {
        // print only when the distance changes
        for (int i = 0; i < sensor_max; i++) {
            int new_mm = sensor[i].dist_mm();
            // sensor[0] sees the other end of the house at ~200 mm; ignore that
            if (i == 0 && new_mm > 180)
                new_mm = INT_MAX;
            if (new_mm != last_mm[i]) {
                if (new_mm == INT_MAX)
                    printf("%d: nothing detected\n", i);
                else
                    printf("%d: dist_mm = %d\n", i, new_mm);
                last_mm[i] = new_mm;
            }
        }
    }

    return 0;
}
