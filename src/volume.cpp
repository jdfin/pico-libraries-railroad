
#include <cstdint>
#include <cstdio>
//
#include "pico/stdlib.h"
//
#include "dcc_api.h"
//
#include "volume.h"


void Volume::loop()
{
    // anything need to change?
    if (_cur_cv_val == _set_cv_val)
        return;

    // is it time to change?
    if ((int32_t(time_us_32()) - _set_us) < 0)
        return;

    if (_change_us == 0) {
        // change in one step
        _cur_cv_val = _set_cv_val;
    } else {
        // change gradually
        if (_cur_cv_val < _set_cv_val) {
            _cur_cv_val += _step_cnt;
            if (_cur_cv_val > _set_cv_val)
                _cur_cv_val = _set_cv_val;
        } else {
            _cur_cv_val -= _step_cnt;
            if (_cur_cv_val < _set_cv_val)
                _cur_cv_val = _set_cv_val;
        }
    }

    printf("cv%d = %d ... ", _cv_num, _cur_cv_val);
    int32_t t0_us = time_us_32();
    DccApi::Status s = DccApi::loco_cv_val_set(_loco_id, _cv_num, _cur_cv_val);
    int32_t t1_us = time_us_32();
    printf("%s (%ld us)\n", s == DccApi::Status::Ok ? "ok" : "ERROR", t1_us - t0_us);

    _set_us += _change_us;

} // void Volume::loop()
