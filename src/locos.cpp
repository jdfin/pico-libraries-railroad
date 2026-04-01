
#include <cstring>
// dcc
#include "dcc_api.h"
using Status = DccApi::Status;
//
#include "locos.h"

static const Loco::Speed speeds_ml560[] = {
    {5, 18},   {10, 36},  {15, 54},  {20, 72},  {25, 88},  {30, 103}, {35, 121},
    {40, 136}, {45, 154}, {50, 172}, {55, 189}, {60, 204}, {65, 224}, {70, 257},
    {75, 288}, {80, 305}, {85, 313}, {90, 317}, {95, 321}, {0, 0},
};

static const Loco::Speed speeds_up852[] = {
    {5, 8},    {10, 17},   {15, 26},   {20, 37},  {25, 48},  {30, 59},
    {35, 72},  {40, 86},   {45, 100},  {50, 117}, {55, 133}, {60, 149},
    {65, 166}, {70, 185},  {75, 205},  {80, 224}, {85, 243}, {90, 260},
    {95, 266}, {100, 270}, {105, 274}, {0, 0},
};

static const Loco::Speed speeds_mp1130[] = {
    // doesn't always move at dcc=5
    {7, 13},   {10, 24},   {15, 30},  {20, 41},  {25, 52},  {30, 65},
    {35, 79},  {40, 94},   {45, 110}, {50, 128}, {55, 145}, {60, 164},
    {65, 183}, {70, 205},  {75, 228}, {80, 252}, {85, 272}, {90, 278},
    {95, 280}, {100, 283}, {0, 0},
};

static const Loco::Speed speeds_sp2265[] = {
    {5, 9},    {10, 18},   {15, 28},   {20, 40},   {25, 54},  {30, 67},
    {35, 81},  {40, 95},   {45, 111},  {50, 128},  {55, 144}, {60, 163},
    {65, 183}, {70, 202},  {75, 224},  {80, 245},  {85, 268}, {90, 291},
    {95, 314}, {100, 329}, {105, 337}, {110, 341}, {0, 0},
};

static Loco locos[] = {
    {
        .sn = 4195122288,
        .name = "ML560",
        .len_mm = 80,
        .speeds = speeds_ml560,
        .stop_mm_num = 1,
        .stop_mm_den = 2,
        .f_headlight = 0,
        .f_cab_light = 1,
        .f_bell = -1,
        .f_horn = -1,
        .f_engine = -1,
        .f_squeal = -1,
        .f_clank = -1,
        .v_master = -1,
        .v_engine = -1,
    },
    {
        .sn = 3757438420,
        .name = "UP852",
        .len_mm = 200,
        .speeds = speeds_up852,
        .stop_mm_num = 3,
        .stop_mm_den = 4,
        .f_headlight = 0,
        .f_cab_light = -1,
        .f_bell = 1,
        .f_horn = 2,
        .f_engine = 8,
        .f_squeal = -1, // might have it, just not mapped
        .f_clank = 3,
        .v_master = 20,
        .v_engine = 64,
    },
    {
        .sn = 3757749420,
        .name = "MP1130",
        .len_mm = 160,
        .speeds = speeds_mp1130,
        .stop_mm_num = 2, // not measured
        .stop_mm_den = 3, // not measured
        .f_headlight = 0,
        .f_cab_light = 16,
        .f_bell = 1,
        .f_horn = 2,
        .f_engine = 8,
        .f_squeal = 3,
        .f_clank = -1,
        .v_master = 40,
        .v_engine = 64,
    },
    {
        .sn = 4192888681,
        .name = "SP2265",
        .len_mm = 160,
        .speeds = speeds_sp2265,
        .stop_mm_num = 2,
        .stop_mm_den = 3,
        .f_headlight = 0,
        .f_cab_light = -1,
        .f_bell = 1,
        .f_horn = 2,
        .f_engine = 8,
        .f_squeal = 7,
        .f_clank = 3,
        .v_master = 40,
        .v_engine = 64, // default=192
    },
};

static constexpr int locos_max = sizeof(locos) / sizeof(locos[0]);


Status Loco::read_sn(int loco_id, uint32_t &sn, int attempts)
{
    Status s;

    // set cv31/cv32 for railcom page

    s = DccApi::loco_cv_val_set(loco_id, 31, 0, attempts);
    if (s != Status::Ok)
        return s;

    s = DccApi::loco_cv_val_set(loco_id, 32, 255, attempts);
    if (s != Status::Ok)
        return s;

    // read sn from cv 265-268 (little-endian)
    sn = 0;
    for (int cv_num = 268; cv_num >= 265; cv_num--) {
        int cv_val;
        s = DccApi::loco_cv_val_get(loco_id, cv_num, cv_val, attempts);
        if (s != Status::Ok)
            break;
        sn = (sn << 8) | cv_val;
    }

    return Status::Ok;
}


// given a (desired) speed in mm/s, return the dcc speed that achieves that
int Loco::speed_dcc(int mms) const
{
    int dir = +1;
    if (mms < 0) {
        mms = -mms;
        dir = -1;
    }

    // slower than slowest entry?
    if (mms <= speeds[0].mms)
        return dir * speeds[0].dcc; // return slowest we can go

    int i = 0;
    while (speeds[i + 1].mms != 0) {
        // between this entry and the next?
        if (speeds[i].mms <= mms && mms <= speeds[i + 1].mms) {
            // return interpolated dcc speed
            const int d_dcc = speeds[i + 1].dcc - speeds[i].dcc;
            const int d_mms = speeds[i + 1].mms - speeds[i].mms;
            const int o_mms = mms - speeds[i].mms;
            return dir * (speeds[i].dcc + (d_dcc * o_mms + d_mms / 2) / d_mms);
        }
        i++;
    }
    return dir * speeds[i].dcc; // return fastest we can go
}

// given a speed in dcc, return the actual speed in mm/s
int Loco::speed_mms(int dcc) const
{
    int dir = +1;
    if (dcc < 0) {
        dcc = -dcc;
        dir = -1;
    }

    if (dcc <= speeds[0].dcc)
        return dir * speeds[0].mms;
    int i = 0;
    while (speeds[i + 1].dcc != 0) {
        if (speeds[i].dcc <= dcc && dcc <= speeds[i + 1].dcc) {
            // return interpolated mms speed
            const int d_dcc = speeds[i + 1].dcc - speeds[i].dcc;
            const int d_mms = speeds[i + 1].mms - speeds[i].mms;
            const int o_dcc = dcc - speeds[i].dcc;
            return dir * (speeds[i].mms + (d_mms * o_dcc + d_dcc / 2) / d_dcc);
        }
        i++;
    }
    return dir * speeds[i].mms;
}

// find stop distance in mm from speed in mm/s (when cv4 = 0)
// d(mm) = s(mm/s) * stop_mm_num / stop_mm_den
int Loco::stop_mm(int mms) const
{
    return (mms * stop_mm_num + stop_mm_den / 2) / stop_mm_den;
}

const Loco *Loco::find_loco(uint32_t sn)
{
    for (int i = 0; i < locos_max; i++) {
        if (sn == locos[i].sn)
            return &locos[i];
    }
    return nullptr;
}

const Loco *Loco::find_loco(const char *name)
{
    for (int i = 0; i < locos_max; i++) {
        if (strcmp(name, locos[i].name) == 0)
            return &locos[i];
    }
    return nullptr;
}
