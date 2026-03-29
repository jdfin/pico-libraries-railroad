
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
#include "sensor2.h"


//  PWM            +-----| USB |-----+            PWM
//   0A         D0 | 1            40 | VBUS_OUT
//   0B         D1 | 2            39 | VSYS_IO
//             GND | 3            38 | GND
//   1A         D2 | 4            37 | 3V3_EN
//   1B         D3 | 5            36 | 3V3_OUT
//   2A         D4 | 6            35 | AREF
//   2B         D5 | 7            34 | A2/D28     6A
//             GND | 8            33 | GND
//   3A         D6 | 9            32 | A1/D27     5B
//   3B         D7 | 10           31 | A0/D26     5A
//   4A         D8 | 11           30 | RUN
//   4B         D9 | 12           29 | D22        3A
//             GND | 13           28 | GND
//   5A        D10 | 14           27 | D21        2B
//   5B        D11 | 15           26 | D20        2A
//   6A        D12 | 16           25 | D19        1B
//   6B        D13 | 17           24 | D18        1A
//             GND | 18           23 | GND
//   7A        D14 | 19           22 | D17        0B
//   7B        D15 | 20           21 | D16        0A
//                 +-----------------+

// Sensor has to be a PWM B channel.
static constexpr int sensor_gpio = 15;
static Sensor2 sensor(sensor_gpio);


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

    sensor.init();

    int last_mm = INT_MAX;
    while (true) {
        // print only when the distance changes
        int new_mm = sensor.dist_mm();
        if (new_mm != last_mm) {
            if (new_mm < 0)
                printf("nothing detected\n");
            else
                printf("dist_mm = %d\n", new_mm);
            last_mm = new_mm;
        }
    }

    return 0;
}
