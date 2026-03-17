#pragma once

#include <cstdint>
//
#include "pico/stdlib.h"
#include "pico/time.h"
//
#include "dcc_api.h"


class AFunc
{
public:

    AFunc(int afunc_max = 20) :
        _afunc_max(afunc_max),
        _put(0),
        _get(0)
    {
        _afunc = new OneFunc[_afunc_max];
        assert(_afunc != nullptr);
    }

    // schedule a function change at a given time
    bool put(uint32_t time_us, int loco, int func, bool on)
    {
        if (full())
            return false;
        _afunc[_put].time_us = int32_t(time_us);
        _afunc[_put].loco = loco;
        _afunc[_put].func = func;
        _afunc[_put].on = on;
        _put = next(_put);
        return true;
    }

    void loop()
    {
        int32_t now_us = int32_t(time_us_32());
        while (!empty() && (now_us - _afunc[_get].time_us) >= 0) {
            // set the function
            int loco = _afunc[_get].loco;
            int func = _afunc[_get].func;
            bool on = _afunc[_get].on;
            DccApi::loco_func_set(loco, func, on);
            _get = next(_get);
        }
    }

private:

    int _afunc_max;

    struct OneFunc {
        // use signed so "now >= time" is easier in the case of timer wrap (every 71 minutes)
        int32_t time_us;
        int loco;
        int func;
        bool on;
    };

    OneFunc *_afunc;

    // _put is the next entry to be added, always an empty slot
    // _get is the next entry to be run, always a valid entry
    // _put == _get means empty
    // _put+1 == _get means full (last entry can't be used)
    int _put;
    int _get;

    bool full() const
    {
        return next(_put) == _get;
    }

    bool empty() const
    {
        return _put == _get;
    }

    int next(int idx) const
    {
        return (idx + 1) % _afunc_max;
    }

}; // class AFunc
