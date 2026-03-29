
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
// misc
#include "sys_led.h"
// sensors
#include "sensor.h"

static constexpr int sensor_max = 8;

static constexpr int gpio[sensor_max] = {13, 6, 7, 8, 9, 10, 11, 12};

static Sensor sensor[sensor_max] = {
    Sensor(gpio[0]), Sensor(gpio[1]), Sensor(gpio[2]), Sensor(gpio[3]),
    Sensor(gpio[4]), Sensor(gpio[5]), Sensor(gpio[6]), Sensor(gpio[7]),
};


int main()
{
    stdio_init_all();

    SysLed::init();
    SysLed::pattern(50, 950);

    while (!stdio_usb_connected()) {
        tight_loop_contents();
        SysLed::loop();
    }

    sleep_ms(10);

    SysLed::off();

    printf("\n");
    printf("sensors_test\n");
    printf("\n");

    bool prev_state[sensor_max];

    for (int s = 0; s < sensor_max; s++)
        prev_state[s] = sensor[s];

    while (true) {
        for (int s = 0; s < sensor_max; s++) {
            if (sensor[s] != prev_state[s]) {
                printf("sensor %d %s\n", s, sensor[s] ? "on" : "off");
                prev_state[s] = sensor[s];
            }
        }
    }

    return 0;
}
