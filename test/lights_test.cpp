
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h" // sleep_ms
// railroad
#include "railroad/config.h"
// ws2812
#include "ws2812/ws2812.h"
// railroad
#include "railroad/lights.h"

// lights is a strip of ws2812; use four of them
static Ws2812 ws2812(ws2812_gpio, 4);
static constexpr int house_brt = 255;
static Lights lights(ws2812, house_brt);

static void init();
static void loop_ms(int32_t for_ms = 0);


inline int32_t time_us_32s()
{
    return int32_t(time_us_32());
}


int main()
{
    stdio_init_all();

#if 0
    while (!stdio_usb_connected())
        tight_loop_contents();
#endif

    printf("\n");
    printf("lights_test\n");
    printf("\n");

    init();

    lights.off();

    const int32_t delay_ms = 3000;

    // slow and fast change times
    const int32_t slow_ms = 2000;
    const int32_t fast_ms = 250;

    int32_t change_ms = slow_ms;

    while (true) {

        // alternate between fast and slow
        change_ms = (change_ms == slow_ms) ? fast_ms : slow_ms;
        lights.set_change_us((change_ms * 1'000 + house_brt / 2) / house_brt);

        lights.red();
        loop_ms(delay_ms);

        lights.green();
        loop_ms(delay_ms);

        lights.blue();
        loop_ms(delay_ms);

        lights.white();
        loop_ms(delay_ms);

        lights.off();
        loop_ms(delay_ms);
    }

    sleep_ms(100);

    return 0;

} // main()


static void init()
{

    ws2812.init();
    lights.off();

} // init()


static void loop_ms(int32_t for_ms)
{

    int32_t end_us = time_us_32s() + 1000 * for_ms;
    while (end_us - time_us_32s() >= 0) {
        lights.loop();
        tight_loop_contents();
    }

} // loop_ms()
