#pragma once

#include <cassert>
#include <climits>
#include <cstdio>

#include "pico/stdlib.h"
//
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
//
#include "gpio_extra.h"


class Sensor2
{

public:

    Sensor2(int gpio) :
        _gpio(gpio),
        _slice(pwm_gpio_to_slice_num(gpio)),
        _count{0},
        _count_idx(0),
        _cb(nullptr),
        _cb_arg(0)
    {
    }

    void init()
    {
        // set up pin for falling edge interrupt
        gpio_init(_gpio);
        gpiox_set_callback(_gpio, irq_handler, intptr_t(this));
        gpio_set_irq_enabled(_gpio, GPIO_IRQ_EDGE_FALL, true);

        // Clock the pwm at 1 MHz for 1 usec resolution       Pico2     Pico
        const uint32_t sys_clk_hz = clock_get_hz(clk_sys); // 150 MHz   125 MHz
        const uint32_t pwm_clk_hz = 1'000'000;             //
        const uint32_t pwm_div = sys_clk_hz / pwm_clk_hz;  // 150       125
        assert(pwm_div <= 255); // must fit in 8 bit register

        // set up pwm with pin as counter gate
        gpio_set_function(_gpio, GPIO_FUNC_PWM);
        pwm_set_enabled(_slice, false);
        pwm_config pc = pwm_get_default_config();
        pwm_config_set_clkdiv_mode(&pc, PWM_DIV_B_HIGH); // count while pin high
        pwm_config_set_clkdiv_int(&pc, pwm_div);
        pwm_init(_slice, &pc, false);
        uint chan = pwm_gpio_to_channel(_gpio);
        assert(chan == PWM_CHAN_B);
        pwm_set_chan_level(_slice, chan, 0);
        pwm_set_enabled(_slice, true);
    }

    // return most recent count, no averaging
    uint16_t count() const
    {
        int idx = _count_idx;
        if (idx < 0)
            idx = avg_len - 1;
        return _count[idx];
    }

    int dist_mm() const
    {
        // Pololu 4064:
        // * pulse width of 2000_us means nothing detected
        // * d_mm = 3_mm / 4_us * (t_us - 1000_us)

        // If everything in _count[] is a valid reading, return the average,
        // else return "nothing detected".

        uint16_t sum = 0;
        constexpr int count_margin = 10;
        for (int i = 0; i < avg_len; i++) {
            if (_count[i] < (1000 - count_margin) || _count[i] > (2000 - count_margin))
                return INT_MAX; // "nothing detected"
            sum += _count[i];
        }

        int avg = (sum + avg_len / 2) / avg_len; // round to nearest

        return (avg - 1000) * 3 / 4;
    }

    uint slice() const
    {
        return _slice;
    }

    void set_callback(void (*cb)(uint16_t count, intptr_t arg), intptr_t arg)
    {
        gpio_set_irq_enabled(_gpio, GPIO_IRQ_EDGE_FALL, false);
        _cb = cb;
        _cb_arg = arg;
        gpio_set_irq_enabled(_gpio, GPIO_IRQ_EDGE_FALL, true);
    }

private:

    int _gpio;
    uint _slice;
    static constexpr int avg_len = 4;
    uint16_t _count[avg_len];
    int _count_idx;

    // callback for every count update, every 9-20 msec
    void (*_cb)(uint16_t count, intptr_t arg);
    intptr_t _cb_arg;

    // called at interrupt level
    void handler()
    {
        uint16_t new_count = pwm_get_counter(_slice);
        pwm_set_counter(_slice, 0);
        _count_idx = (_count_idx + 1) % avg_len;
        _count[_count_idx] = new_count;
        if (_cb != nullptr)
            _cb(new_count, _cb_arg);
    }

    // There is one static handler for all sensors. Each sensor attaches it
    // as the callback for its pin with a different arg to identify itself.
    static void irq_handler(uint gpio, uint32_t events, intptr_t arg)
    {
        Sensor2 *sensor = (Sensor2 *)arg;
        assert(int(gpio) == sensor->_gpio);
        assert(events == GPIO_IRQ_EDGE_FALL);
        sensor->handler();
    }
};
