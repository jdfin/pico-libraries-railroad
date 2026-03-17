
#include <cstdint>
//
#include "pico/stdlib.h"
//
#include "hardware/gpio.h"
#include "hardware/pwm.h"
//
#include "turnout.h"


bool Turnout::ready = false;
uint16_t Turnout::pwm_level = 0;
uint Turnout::slice = 0;
uint Turnout::channel = 0;
Turnout* Turnout::turnout[Turnout::max_turnouts] = { nullptr };
int Turnout::num_turnouts = 0;
uint64_t Turnout::pulse_time_us = 0;


Turnout::Turnout(int a, int b) :
    _a(a),
    _b(b)
{

    gpio_init(_a);
    gpio_put(_a, 0);
    gpio_set_dir(_a, GPIO_OUT);

    gpio_init(_b);
    gpio_put(_b, 0);
    gpio_set_dir(_b, GPIO_OUT);

    assert(num_turnouts < max_turnouts);
    turnout[num_turnouts++] = this;
}


void Turnout::init(int p) // static
{
    // 100 msec pulses are close to the longest we can do (with clk_sys).
    // This might not work for faster clocks.
    //                                                pico2     pico
    const uint32_t sys_hz = clock_get_hz(clk_sys); // 150 MHz   125 MHz
    const uint32_t pwm_div = 250;                  //
    const uint32_t pwm_hz = sys_hz / pwm_div;      // 600 KHz   500 KHz
    pwm_level = pwm_hz / 10;                       // 60'000    50'000
                                                   //     100 msec
    gpio_set_function(p, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(p);
    channel = pwm_gpio_to_channel(p);
    pwm_set_enabled(slice, false);
    pwm_config pc = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&pc, pwm_div);
    pwm_init(slice, &pc, false);
    pwm_set_wrap(slice, UINT16_MAX);

    ready = true;
}
