
#include <strings.h>
//
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h"
// misc
#include "argv.h"
#include "str_ops.h"
#include "sys_led.h"
// turnouts
#include "turnout.h"

static constexpr int gpio_a0 = 0;
static constexpr int gpio_b0 = 1;
static constexpr int gpio_a1 = 2;
static constexpr int gpio_b1 = 3;
static constexpr int gpio_p = 4;

static Turnout t0(gpio_a0, gpio_b0);
static Turnout t1(gpio_a1, gpio_b1);

// A command is a sequence of tokens, ending with newline.
//
// The "Argv" object takes a character at a time and builds a complete command
// (an array of tokens), which can then be examined for commands and parameters.

static Argv argv;


static void help()
{
    printf("commands: \"0A\", \"0B\", \"1A\", \"1B\" \"C0\"\n");
}


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
    printf("turnouts_cmd\n");
    printf("\n");
    help();
    printf("\n");

    argv.verbosity(1);

    Turnout::init(gpio_p);

    while (true) {

        // Get console input if available.
        // This might result in a new command.
        int c = stdio_getchar_timeout_us(0);
        if (0 <= c && c <= 255) {
            if (argv.add_char(char(c))) {
                // newline received, try to process command
                if (argv.argc() == 1) {
                    if (strcasecmp(argv[0], "0A") == 0) {
                        t0.set(true);
                    } else if (strcasecmp(argv[0], "0B") == 0) {
                        t0.set(false);
                    } else if (strcasecmp(argv[0], "1A") == 0) {
                        t1.set(true);
                    } else if (strcasecmp(argv[0], "1B") == 0) {
                        t1.set(false);
                    } else if (strcasecmp(argv[0], "C0") == 0) {
                        for (int i = 0; i < 5; i++) {
                            t0.set(true, true);
                            t1.set(true, true);
                            t0.set(false, true);
                            t1.set(false, true);
                        }
                    } else {
                        printf("unrecognized command: ");
                        argv.print();
                        help();
                    }
                } else {
                    help();
                }
                argv.reset();
            }
        }
    } // while (true)

    return 0;
}
