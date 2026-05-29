
#include <cassert>
#include <cstdio>
#include <cstdint>
// dcc
#include "dcc_api.h"
// railroad
#include "locos.h"
#include "desktop_layout.h"
#include "desktop_ops.h"


namespace DesktopOps {


void _nop(int32_t) {}

void (*_loop)(int32_t us) = _nop;

int creep_mms = 20;
int slow_mms = 75;
int medium_mms = 100;
int fast_mms = 150;
int zippy_mms = 300;

int uncouple_fail_cnt = 0;

int car_len_mm = 150; // boxcar and tanker


void set_loop(void (*loop)(int32_t us))
{
    _loop = loop;
}


// How many microseconds to go dist_mm at speed_mms
int32_t mm_to_us(int dist_mm, int speed_mms)
{
    const int32_t t_us = (dist_mm * 1'000'000 + speed_mms / 2) / speed_mms;
    return t_us;
}


// On entry:
// * Loco in house
// * Car on spur (in range of sensor but not too close to it)
// On return:
// *
bool fetch(int loco_id, const Loco *loco, int spur_num)
{
    printf("fetch(%d)\n", spur_num);

    Desktop::line_turnout_0(spur_num);

    // slow out of house
    printf("fetch(%d)[%d]: %d mm/s\n", spur_num, __LINE__, slow_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-slow_mms));
    _loop(mm_to_us(150, slow_mms));

    Desktop::line_turnout_1(spur_num);

    // medium to uncoupler
    printf("fetch(%d)[%d]: %d mm/s\n", spur_num, __LINE__, medium_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-medium_mms));
    while (!Desktop::sensor_unc())
        _loop(0);

    // Rear of loco has reached uncoupler now; go most of the way.
    // If the car is 100 mm from the end, and is car_len_mm long, this should
    // get us to 100 mm from the car.
    int most_mm = Desktop::unc_to_spur_mm(spur_num) - 100 - car_len_mm - 100;
    _loop(mm_to_us(most_mm, medium_mms));

    // creep back until we get the car (until the car moves)
    printf("fetch(%d)[%d]: %d mm/s\n", spur_num, __LINE__, creep_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));
    const int dist_mm = Desktop::sensor_spur(spur_num).dist_mm();
    printf("fetch(%d)[%d]: car at %d mm\n", spur_num, __LINE__, dist_mm);
    constexpr int bump_mm = 15;
    while (Desktop::sensor_spur(spur_num).dist_mm() > (dist_mm - bump_mm))
        _loop(0);

    printf("fetch(%d)[%d]: stop\n", spur_num, __LINE__);
    DccApi::loco_speed_set(loco_id, stop);
    _loop(1'000'000);

    const int bumped_to_mm = Desktop::sensor_spur(spur_num).dist_mm();
    printf("fetch(%d)[%d]: bumped to %d mm\n", spur_num, __LINE__, bumped_to_mm);

    if (loco->f_clank >= 0) {
        DccApi::loco_func_set(loco_id, loco->f_clank, true);
        _loop(1'000'000);
        DccApi::loco_func_set(loco_id, loco->f_clank, false);
    }
    _loop(1'000'000);

    // pull forward a little and make sure the car moves (that it is coupled)
    printf("fetch(%d)[%d]: %d mm/s\n", spur_num, __LINE__, creep_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));
    _loop(mm_to_us(4 * bump_mm, creep_mms));
    const int pulled_to_mm = Desktop::sensor_spur(spur_num).dist_mm();
    printf("fetch(%d)[%d]: pulled to %d mm\n", spur_num, __LINE__, pulled_to_mm);
    if ((pulled_to_mm - bumped_to_mm) < bump_mm) {
        // did not get it!
        printf("fetch(%d)[%d]: did not couple!\n", spur_num, __LINE__);
        return false;
    } else {
        return true;
    }

} // bool fetch(int loco_id, const Loco *loco, int spur_num)


// On entry:
// * Loco+car right of uncoupler, coupled
// On return:
// * Loco left of uncoupler, coupler clear of magnet
// * Car just right of uncoupler with coupler over magnet
void uncouple(int loco_id, const Loco *loco)
{
    printf("uncouple\n");

    // Forward slow until nose of loco is at uncoupler.
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(slow_mms));
    while (!Desktop::sensor_unc())
        _loop(0);
    printf("uncouple[%d]: nose\n", __LINE__);

    bool uncouple_ok = false;

    do {

        // Creep forward until rear of loco (gap) is at uncoupler.
        // We'll either see the gap, or miss the gap and get to the end of the car.
        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));
        while (Desktop::sensor_unc())
            _loop(0);

        // Uncoupler is at gap between loco and car, or at end of car.

        // A bit more to get couplers clear of magnet (~50 mm).
        int more_mm = 50 - loco->stop_mm(creep_mms);
        if (more_mm > 0)
            _loop(mm_to_us(more_mm, creep_mms));
        printf("uncouple[%d]: stop\n", __LINE__);
        DccApi::loco_speed_set(loco_id, stop);
        _loop(1'000'000);

        // If there's something in front of the uncoupler, we saw the gap and it's
        // the car. If there is not something in front of the uncoupler, we missed
        // the gap and the whole train is left of the uncoupler.

        if (Desktop::sensor_unc()) {
            // Saw the gap
            printf("uncouple[%d]: saw gap\n", __LINE__);
        } else {
            // Missed the gap, back up so car is in front of uncoupler
            printf("uncouple[%d]: missed gap\n", __LINE__);
        }

        // Either way, couplers should be left of magnet now.
        // If we missed the gap, we just have to back up farther.

        // Creep back until couplers are over magnet.
        printf("uncouple[%d]: creep back\n", __LINE__);

        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));

        // If there's not something in front of the uncoupler at the start of
        // the creep, the whole train is left of it, so back up until it sees
        // something.
        while (!Desktop::sensor_unc())
            _loop(0);

        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));

        // Look for the gap. If we miss it we'll get to the front of the loco.
        while (Desktop::sensor_unc())
            _loop(0);

        printf("uncouple[%d]: stop\n", __LINE__);
        DccApi::loco_speed_set(loco_id, stop);
        _loop(500'000);

        // Either the couplers are over the magnet, or we missed the gap and
        // the whole train is right of the uncoupler. Stopping distance at
        // 20 mm/sec is typically 10-15 mm, so if we found the gap, we might
        // or might not see the loco at this point.

        // Pull forward to uncouple (should leave car behind).
        // We want to go far enough to:
        // 1) verify we're leaving the car behind, and
        // 2) get the loco's coupler off the magnet.
        printf("uncouple[%d]: creep forward\n", __LINE__);
        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));
        _loop(mm_to_us(60, creep_mms));
        printf("uncouple[%d]: stop\n", __LINE__);
        DccApi::loco_speed_set(loco_id, stop);
        _loop(500'000);
        // uncoupling worked if there's nothing there now
        uncouple_ok = !Desktop::sensor_unc();

        // Possibilities:
        // 1) Successful uncouple: There's nothing in front of uncoupler.
        //    Car is right of uncoupler with it's coupler over the magnet, and
        //    loco is left of uncoupler, clear of magnet.
        // 2) Missed gap when backing: Loco is in front of uncoupler, coupled
        //    to car behind it.
        // 3) Found gap but did not uncouple: Car is in front of uncoupler.

        // retry if necessary
        if (!uncouple_ok) {
            uncouple_fail_cnt++;
            printf("uncouple[%d]: failed (%d), retrying...\n", __LINE__, uncouple_fail_cnt);
        }

    } while (!uncouple_ok);

    printf("uncouple done\n");

} // void uncouple(int loco_id, const Loco *loco)


// On entry:
// * Loco should be left of uncoupler, coupler clear of magnet
// * Car should be right of uncoupler, with its coupler over the magnet
// On return:
// * Loco & car on spur, not coupled
// * Loco creeping forward
// Errors:
// * Sometimes the cars recouple on the way back (esp. the tank car to
//   spur 2). If that happens, we see the car pulling away after it was
//   supposedly left behind.
// Return:
// *  true if the car was left behind
// *  false if the car is still coupled
bool spot(int loco_id, const Loco *loco, int spur_num)
{
    printf("spot(%d)\n", spur_num);

    Desktop::line_turnout_0(spur_num);

    if (loco->f_bell >= 0)
        DccApi::loco_func_set(loco_id, loco->f_bell, true);

    _loop(1'000'000);

    // creep back until loco clears uncoupler
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));
    _loop(mm_to_us(100, creep_mms));
    while (Desktop::sensor_unc())
        _loop(0);

    Desktop::line_turnout_1(spur_num);

    // Spur 2 has an s-turn that can cause a recoupling or even derail, both
    // observed with UP852 and the tank car, but never (yet) with any other
    // loco or the boxcar.
    if (spur_num != 2) {
        // spur 1 or 3, a bit faster most of the way
        // subtract loco and car len, plan to leave it 100 mm from the end,
        // and we'll start creeping 100 mm before that
        int slow_mm =
            Desktop::unc_to_spur_mm(spur_num) - loco->len_mm - car_len_mm - 100 - 100;
        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-slow_mms));
        _loop(mm_to_us(slow_mm, slow_mms));
    }

    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));

    // stop when close enough to the end
    constexpr int stop_mm = 75; // stop this far from the sensor
    int detected_mm = 0;        // where sensor first detected car
    int last_mm = 0;            // last reading before stopping
    do {
        last_mm = Desktop::sensor_spur(spur_num).dist_mm();
        if (detected_mm == 0 && last_mm <= 500)
            detected_mm = last_mm;
        _loop(0);
    } while (last_mm > stop_mm);

    DccApi::loco_speed_set(loco_id, stop);
    _loop(1'000'000);

    if (loco->f_bell >= 0)
        DccApi::loco_func_set(loco_id, loco->f_bell, false);

    printf("spot(%d): detected at %d mm, stopped at %d mm, left at %d mm\n",
           spur_num, detected_mm, last_mm, Desktop::sensor_spur(spur_num).dist_mm());

    if (loco->f_clank >= 0) {
        DccApi::loco_func_set(loco_id, loco->f_clank, true);
        _loop(1'000'000);
        DccApi::loco_func_set(loco_id, loco->f_clank, false);
    }
    _loop(1'000'000);

    // Make sure the car is left behind; creep ahead a bit and make sure
    // the car does not move.
    int dist_mm = Desktop::sensor_spur(spur_num).dist_mm();
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));
    _loop(mm_to_us(75, creep_mms)); // 30 mm is not enough
    // If the car did not move too much, it is not coupled; return true.
    int now_mm = Desktop::sensor_spur(spur_num).dist_mm();
    bool okay = ((now_mm != Sensor2::infinity) && ((now_mm - dist_mm) < 25));

    printf("spot(%d): returning %s (%d mm to %d mm)\n", spur_num,
           okay ? "true" : "false", dist_mm, now_mm);

    return okay;

} // bool spot(int loco_id, const Loco *loco, int spur_num)


// On entry:
// * Loco right of uncoupler
// On return:
// * Loco in house
void home(int loco_id, const Loco *loco)
{
    printf("home\n");

    printf("home[%d]: %d mm/s\n", __LINE__, zippy_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(zippy_mms));
    while (!Desktop::sensor_unc())
        _loop(0);

    printf("home[%d]: %d mm/s\n", __LINE__, fast_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(fast_mms));
    _loop(mm_to_us(75, fast_mms));

    printf("home[%d]: %d mm/s\n", __LINE__, medium_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(medium_mms));
    _loop(mm_to_us(100, medium_mms));

    printf("home[%d]: %d mm/s\n", __LINE__, slow_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(slow_mms));
    constexpr int creep_at_mm = 150;
    while (Desktop::sensor_home().dist_mm() > creep_at_mm)
        _loop(0);

    printf("home[%d]: %d mm/s\n", __LINE__, creep_mms);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));
    constexpr int stop_at_mm = 35;
    while (Desktop::sensor_home().dist_mm() > stop_at_mm)
        _loop(0);

    printf("home[%d]: stop\n", __LINE__);
    DccApi::loco_speed_set(loco_id, stop);
    _loop(500'000);

    printf("home done\n");

} // void home(int loco_id, const Loco *loco)


}; // namespace DesktopOps
