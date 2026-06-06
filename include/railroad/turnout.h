#pragma once

#include <cassert>
#include <cstdint>
//
#include "pico/stdlib.h"
//
#include "hardware/clocks.h" // clock_get_hz
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// All turnouts share the same "pulse" pin.
// When the pin is pulsed, only one turnout should have its "a" or "b" pin set.


class Turnout
{
public:

    Turnout(int a, int b);

    // initialze shared pulse pin
    static void init(int p);

    bool set(bool a, bool wait = true)
    {
        assert(ready);

        if (!charged(wait))
            return false;

        // set this turnout to move on the pulse and
        // all the others so they do not move
        for (int i = 0; i < num_turnouts; i++) {
            if (turnout[i] == this) {
                gpio_put(_a, a);
                gpio_put(_b, !a);
            } else {
                gpio_put(turnout[i]->_a, 0);
                gpio_put(turnout[i]->_b, 0);
            }
        }
        start_pulse();
        return true;
    }

private:

    int _a;
    int _b;

    static constexpr uint32_t pulse_dur_us = 100'000;
    static constexpr uint32_t recharge_dur_us = 500'000;

    static bool ready;

    static uint16_t pwm_level;

    static uint slice;   // uint to match pico-sdk
    static uint channel; // uint to match pico-sdk

    static constexpr int max_turnouts = 10;
    static Turnout *turnout[max_turnouts];
    static int num_turnouts;

    // time of last pulse, to enforce recharge
    // (all turnouts share the same capacitor)
    static uint64_t pulse_time_us;

    // clear a and b so turnout does not move on pulse
    void idle()
    {
        gpio_put(_a, 0);
        gpio_put(_b, 0);
    }

    // has enough time passed since the last call to any turnout's start_pulse?
    static bool charged(bool wait)
    {
        constexpr uint32_t pulse_int_us = pulse_dur_us + recharge_dur_us;
        if (wait)
            while ((pulse_time_us + pulse_int_us) > time_us_64())
                tight_loop_contents();
        return (pulse_time_us + pulse_int_us) <= time_us_64();
    }

    // start a pulse - returns immediately with the p pin asserted
    static void start_pulse()
    {
        // only called from set(), where these have already been checked
        //assert(ready);
        //assert(charged());

        // start it
        pwm_set_enabled(slice, false);
        pwm_set_counter(slice, 0); // PWM_CHx_CTR_RESET
        pwm_set_chan_level(slice, channel, pwm_level);
        pwm_set_enabled(slice, true);
        pwm_set_chan_level(slice, channel, 0); // takes effect after wrap
        pulse_time_us = time_us_64();
    }

}; // class Turnout
