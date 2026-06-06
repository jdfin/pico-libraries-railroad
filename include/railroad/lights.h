#pragma once

#include <climits>
#include <cstdint>
//
#include "hardware/gpio.h"
//
#include "ws2812/ws2812.h"


class Lights
{

public:

    Lights(Ws2812 &ws2812, uint8_t brt = UINT8_MAX) :
        _ws2812(ws2812),
        _brt(brt),
        // setting _cur=0 and _set=1 makes it so the first call to loop()
        // turns the LEDs off
        _cur_r(0),
        _cur_g(0),
        _cur_b(0),
        _set_r(1),
        _set_g(1),
        _set_b(1),
        _set_us(time_us_32s()),
        _change_us(1'000) // full off to on in 0.255 seconds
    {
    }

    void set_brightness(int brt)
    {
        _brt = brt;
    }

    // When changing the lights, each of red, green, and blue moves from the
    // current setting towards the goal setting by a count of 1 every
    // _change_us.
    //
    // For example, to change from full-brightness red to green in 1 second,
    // we need to step 255 times in 1 second, so change_us should be
    // 1'000'000 / 255 = 3922.
    //
    // Zero means change in one step.
    //
    // The PIO takes 30 usec per LED, and we never want to block, so attempts
    // to set change_us to (30 * _count) or less just set it to zero.
    void set_change_us(int32_t change_us)
    {
        if (change_us <= (30 * _ws2812.count()))
            change_us = 0;
        _change_us = change_us;
    }

    int32_t change_us() const
    {
        return _change_us;
    }

    void set(uint8_t r, uint8_t g, uint8_t b, int delay_ms = 0)
    {
        _set_r = r;
        _set_g = g;
        _set_b = b;
        _set_us = time_us_32s() + 1000 * delay_ms;
    }

    // clang-format off
    inline void off(int delay_ms=0)    { set(   0,    0,    0, delay_ms); }
    inline void red(int delay_ms=0)    { set(_brt,    0,    0, delay_ms); }
    inline void green(int delay_ms=0)  { set(   0, _brt,    0, delay_ms); }
    inline void blue(int delay_ms=0)   { set(   0,    0, _brt, delay_ms); }
    inline void yellow(int delay_ms=0) { set(_brt, _brt,    0, delay_ms); }
    inline void purple(int delay_ms=0) { set(_brt,    0, _brt, delay_ms); }
    inline void cyan(int delay_ms=0)   { set(   0, _brt, _brt, delay_ms); }
    inline void white(int delay_ms=0)  { set(_brt, _brt, _brt, delay_ms); }
    // clang-format on

    // loop() handles gradual and/or delayed changes
    void loop();

private:

    Ws2812 &_ws2812;

    uint8_t _brt;

    // current setting
    uint8_t _cur_r;
    uint8_t _cur_g;
    uint8_t _cur_b;

    // for a future change, what time and what's the goal
    uint8_t _set_r;
    uint8_t _set_g;
    uint8_t _set_b;
    int32_t _set_us;

    // how often we change the lights towards the goal setting
    int32_t _change_us;

    // just so we don't always have to cast...
    inline int32_t time_us_32s()
    {
        return int32_t(time_us_32());
    }

}; // class Lights
