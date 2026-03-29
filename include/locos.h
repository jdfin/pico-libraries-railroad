#pragma once

#include <cstdint>

#include "dcc_api.h"

struct Loco {

    uint32_t sn;
    static DccApi::Status read_sn(int loco_id, uint32_t &sn, int attempts = 5);

    const char *name;

    int len_mm;

    // table to map between dcc speed and actual speed in mm/s
    const struct Speed {
        int dcc;
        int mms;
    } *speeds;
    int speed_dcc(int mms) const;
    int speed_mms(int dcc) const;

    // stop distance in mm from speed in mm/s (when cv4 = 0)
    // d(mm) = s(mm/s) * stop_mm_num / stop_mm_den
    int stop_mm_num;
    int stop_mm_den;
    int stop_mm(int mms) const;

    // function numbers
    int f_headlight;
    int f_cab_light;
    int f_bell;
    int f_horn;
    int f_engine;
    int f_squeal;
    int f_clank;

    // default volume settings
    int v_master;

    static const Loco *find_loco(uint32_t sn);
    static const Loco *find_loco(const char *name);
};
