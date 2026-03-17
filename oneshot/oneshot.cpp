
#include <cstdint>
// pico
#include "pico/stdlib.h"
#include "pico/time.h"
// hardware
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

static constexpr uint gpio_oneshot = 0;
static constexpr uint gpio_debug = 1;


int main()
{
    uint slice = pwm_gpio_to_slice_num(gpio_oneshot);
    uint channel = pwm_gpio_to_channel(gpio_oneshot);

    if (gpio_debug >= 0) {
        gpio_init(gpio_debug);
        gpio_set_dir(gpio_debug, true); // true means out
    }

    gpio_set_function(gpio_oneshot, GPIO_FUNC_PWM);

    // 100 msec pulses are close to the longest we can do (with clk_sys).
    // This might not work for faster clocks.
    //                                                pico2     pico
    const uint32_t sys_hz = clock_get_hz(clk_sys); // 150 MHz   125 MHz
    const uint32_t pwm_div = 250;                  //
    const uint32_t pwm_hz = sys_hz / pwm_div;      // 600 KHz   500 KHz
    const uint16_t pwm_level = pwm_hz / 10;        // 60'000    50'000
                                                   //     100 msec
    pwm_set_enabled(slice, false);
    pwm_config pc = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&pc, pwm_div);
    pwm_init(slice, &pc, false); // init but don't start
    pwm_set_wrap(slice, UINT16_MAX);

    uint32_t tick_next = time_us_32() + 10'000; // 10 msec from now

    while (true) {

        while ((int32_t(time_us_32() - tick_next)) < 0)
            tight_loop_contents();

        // gpio_debug is high for about 500 nsec on pico2
        gpio_set_mask(1u << gpio_debug);
        pwm_set_enabled(slice, false);
        pwm_set_counter(slice, 0); // PWM_CHn_CTR_RESET
        pwm_set_chan_level(slice, channel, pwm_level);
        pwm_set_enabled(slice, true);
        pwm_set_chan_level(slice, channel, 0); // takes effect at wrap
        gpio_clr_mask(1u << gpio_debug);

        tick_next += 120'000; // pulse starts every 120 msec
    }

    return 0;
}
