
#include "lights.h"


void Lights::loop()
{
    // anything need to change?
    if (_set_r == _cur_r && _set_g == _cur_g && _set_b == _cur_b)
        return;

    // is it time to change?
    if ((time_us_32s() - _set_us) < 0)
        return;

    if (_change_us == 0) {
        // change in one step
        _cur_r = _set_r;
        _cur_g = _set_g;
        _cur_b = _set_b;
    } else {
        // change gradually
        if (_set_r > _cur_r)
            _cur_r++;
        else if (_set_r < _cur_r)
            _cur_r--;

        if (_set_g > _cur_g)
            _cur_g++;
        else if (_set_g < _cur_g)
            _cur_g--;

        if (_set_b > _cur_b)
            _cur_b++;
        else if (_set_b < _cur_b)
            _cur_b--;
    }

    _ws2812.set(_cur_r, _cur_g, _cur_b);

    _set_us += _change_us;

} // void Lights::loop()
