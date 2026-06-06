
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h" // sleep_ms
// dcc
#include "dcc/dcc_api.h"
// railroad
#include "railroad/config.h"
#include "railroad/volume.h"

// need one loco with sound on the track

static constexpr int loco_id = 3;
static constexpr int volume_cv_num = 63;
static constexpr int volume_cv_val_default = 128; // after reset (UP 852)
static Volume volume(loco_id, volume_cv_num, volume_cv_val_default);

static void init();
static void loop_ms(int32_t for_ms = 0);


int main()
{
    stdio_init_all();

#if 1
    while (!stdio_usb_connected())
        tight_loop_contents();
    sleep_ms(100);
#endif

    printf("\n");
    printf("volume_test\n");
    printf("\n");

    init();

    const int32_t delay_ms = 20'000;

    // slow and fast change times
    const int32_t slow_ms = 4000;
    const int32_t fast_ms = 1000;

    int32_t change_ms = slow_ms;

    const int vol_hi = 20;
    const int vol_lo = 0;

    const int step_cnt = 1;
    const int vol_steps = (vol_hi - vol_lo) / step_cnt;

    volume.set_step_cnt(step_cnt);

    while (true) {

        // alternate between fast and slow
        change_ms = (change_ms == slow_ms) ? fast_ms : slow_ms;
        int32_t change_us = (change_ms * 1'000 + vol_steps / 2) / vol_steps;
        // If we try to write a cv more often than about every 120 msec,
        // something strange happens. Normally it takes ~20 msec for the
        // response to come back, but it starts taking longer, up to ~120 msec.
        if (change_us < 150'000)
            change_us = 150'000;
        int32_t real_ms = (change_us * vol_steps + 500) / 1000;
        printf("change_ms = %ld; change_us = %ld -> real_ms = %ld\n", change_ms, change_us, real_ms);
        volume.set_change_us(change_us);

        printf("volume = %d\n", vol_lo);
        volume.set(vol_lo);
        loop_ms(delay_ms);

        printf("volume = %d\n", vol_hi);
        volume.set(vol_hi);
        loop_ms(delay_ms);

    } // while (true)

    sleep_ms(100);

    return 0;

} // main()


static void init()
{
    DccApi::init(dcc_bit_gpio, dcc_pwr_gpio, dcc_adc_gpio, dcc_rcom_gpio,
                 dcc_rcom_uart);

    DccApi::Status s;

    printf("reset loco ... ");
    while ((s = DccApi::cv_val_set(8, 8)) != DccApi::Status::Ok) {
        printf("%s ... ", DccApi::status(s));
        loop_ms(1'000);
    }
    printf("ok\n");

    printf("create loco ... ");
    assert(DccApi::loco_create(loco_id) == DccApi::Status::Ok);
    printf("ok\n");

    printf("track on ... ");
    assert(DccApi::track_set(true) == DccApi::Status::Ok);
    printf("ok\n");

    loop_ms(1'000); // wait for loco to boot up

    // disable startup delay
    int cv_num = 124;
    int bit_num = 2;
    int bit_val = 0;
    printf("cv%d[%d] = %d ... ", cv_num, bit_num, bit_val);
    while (true) {
        s = DccApi::loco_cv_bit_set(loco_id, cv_num, bit_num, bit_val);
        if (s == DccApi::Status::Ok)
            break;
        printf("%s ... ", DccApi::status(s));
        loop_ms(1'000);
    }
    printf("ok\n");

    // turn on engine sound
    int f_num = 8;
    printf("f%d on ... ", f_num);
    DccApi::loco_func_set(loco_id, f_num, true);
    printf("ok\n");

} // init()


static void loop_ms(int32_t for_ms)
{

    int32_t end_us = int32_t(time_us_32()) + 1000 * for_ms;
    while (end_us - int32_t(time_us_32()) >= 0) {
        volume.loop();
        tight_loop_contents();
    }

} // loop_ms()
