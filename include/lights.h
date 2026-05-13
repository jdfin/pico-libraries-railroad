#pragma once

#include "hardware/gpio.h"
//
#include "ws2812.h"


class Lights
{

public:

    Lights(Ws2812 &ws2812, int brightness_pct = 100) :
        _ws2812(ws2812),
        _brightness(pct_to_brt(brightness_pct)),
        _set(false)
    {
    }

    // clang-format off
    inline void off(int delay_ms=0)    { set(     0,           0,           0,      delay_ms); }
    inline void red(int delay_ms=0)    { set(_brightness,      0,           0,      delay_ms); }
    inline void green(int delay_ms=0)  { set(      0,     _brightness,      0,      delay_ms); }
    inline void blue(int delay_ms=0)   { set(      0,          0,      _brightness, delay_ms); }
    inline void yellow(int delay_ms=0) { set(_brightness, _brightness,      0,      delay_ms); }
    inline void purple(int delay_ms=0) { set(_brightness,      0,      _brightness, delay_ms); }
    inline void cyan(int delay_ms=0)   { set(      0,     _brightness, _brightness, delay_ms); }
    inline void white(int delay_ms=0)  { set(_brightness, _brightness, _brightness, delay_ms); }
    // clang-format on

    void loop()
    {
        if (_set && (int32_t(time_us_32()) - _set_us) >= 0) {
            _ws2812.set(_set_red, _set_grn, _set_blu);
            _set = false;
        }
    }

private:

    Ws2812 &_ws2812;

    uint8_t _brightness;

    // for a future change, what time and what settings
    bool _set;
    int32_t _set_us;
    uint8_t _set_red;
    uint8_t _set_grn;
    uint8_t _set_blu;

    inline void set(uint8_t red, uint8_t grn, uint8_t blu, int delay_ms = 0)
    {
        if (delay_ms == 0) {
            _ws2812.set(red, grn, blu);
            _set = false;
        } else {
            _set_red = red;
            _set_grn = grn;
            _set_blu = blu;
            _set_us = int32_t(time_us_32()) + 1000 * delay_ms;
            _set = true;
        }
    }

    inline uint8_t pct_to_brt(int pct)
    {
        if (pct <= 0)
            return 0;
        else if (pct >= 100)
            return 255;
        else
            return (pct * 255 + 50) / 100;
    }

}; // class Lights
