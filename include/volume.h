#pragma once

#include <climits>
#include <cstdint>
//
#include "pico/stdlib.h"


// Provide a method to set the desired volume, and a loop function that
// gradually changes the volume to that goal.


class Volume
{

public:

    Volume(int loco_id, int cv_num, int cur_cv_val = 0) :
        _loco_id(loco_id),
        _cv_num(cv_num),
        _cur_cv_val(cur_cv_val),
        _set_cv_val(cur_cv_val),
        _set_us(0),
        _step_cnt(1),
        _change_us(1'000) // 0 to 255 in 0.255 seconds
    {
    }

    // When changing the value, move from the current setting towards the goal
    // setting by a count of 1 every _change_us.
    //
    // For example, to change from zero to 100 in 1 second, we need to step
    // 100 times in 1 second, so change_us should be 1,000,000 / 100 = 10,000.
    // change_us = 0 means change in one step.
    void set_change_us(int32_t change_us)
    {
        assert(change_us >= 0);
        _change_us = change_us;
    }

    int32_t get_change_us() const
    {
        return _change_us;
    }

    int get_step_cnt() const
    {
        return _step_cnt;
    }

    void set_step_cnt(int step_cnt)
    {
        assert(step_cnt > 0);
        _step_cnt = step_cnt;
    }

    void set(int cv_val, int delay_ms = 0)
    {
        assert(cv_val >= -127 && cv_val <= 255);
        _set_cv_val = cv_val;
        _set_us = int32_t(time_us_32()) + 1000 * delay_ms;
    }

    // loop() handles gradual and/or delayed changes
    void loop();

private:

    int _loco_id;

    int _cv_num;

    // current setting
    int _cur_cv_val;

    // for a future change, what time and what's the goal
    int _set_cv_val;
    int32_t _set_us;

    // how much we change the value each time
    int _step_cnt;

    // how often we change the value towards the goal setting
    int32_t _change_us;

}; // class Volume
