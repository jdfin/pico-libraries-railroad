
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
// railroad
#include "railroad/display.h"

static Display display;

static void loop_ms(int32_t for_ms = 0);


int main()
{
    stdio_init_all();

#if 0
    while (!stdio_usb_connected())
        tight_loop_contents();
#endif

    printf("\n");
    printf("display_test\n");
    printf("\n");

    display.init();

    display.set_align(Display::HAlign::Center, Display::VAlign::Center);

    const int32_t delay_ms = 1000;

    while (true) {
        display.show("1 Line");                          loop_ms(delay_ms);
        display.show("\n2 Lines");                       loop_ms(delay_ms);
        display.show("2 Lines\n");                       loop_ms(delay_ms);
        display.show("3 Lines\n\n");                     loop_ms(delay_ms);
        display.show("Hello,\nWorld!");                  loop_ms(delay_ms);
        display.show("Hello,\n\nWorld!");                loop_ms(delay_ms);
        display.show("Hello,\n\n\nWorld!");              loop_ms(delay_ms);
        display.show("Hello,\nWorld!\nSay...\nCHEESE!"); loop_ms(delay_ms * 3);
    }

    sleep_ms(100);

    return 0;

} // main()


static void loop_ms(int32_t for_ms)
{
    int32_t end_us = int32_t(time_us_32()) + 1000 * for_ms;
    while (end_us - int32_t(time_us_32()) >= 0)
        tight_loop_contents();
}
