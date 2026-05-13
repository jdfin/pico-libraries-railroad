
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h" // sleep_ms
// misc
#include "sys_led.h"
// dcc
#include "dcc_api.h"
using Status = DccApi::Status;
// railroad
#include "config.h"
#include "desktop_layout.h"
#include "locos.h"

static constexpr int loco_id = 3;
static const Loco *loco = nullptr;

static void uncoupler_sensor(bool active, intptr_t);
static void loop(int32_t for_us = 0);
static void init();
static void ops_cv_val_set(int cv_num, int cv_val);

static const int creep_mms = 20;

static int32_t creep_mm(int32_t us)
{
    return (creep_mms * us + 500'000) / 1'000'000;
}


int main()
{
    stdio_init_all();
    SysLed::init();
    SysLed::pattern(50, 950);

    while (!stdio_usb_connected())
        loop();
    sleep_ms(10);

    SysLed::off();

    printf("\n");
    printf("sensor_test\n");
    printf("\n");

    init();

    //sensor_unc().set_callback(uncoupler_sensor, 0);

    Desktop::line_turnout_0(1);

    // start with train in front of uncoupler
    while (!Desktop::sensor_unc()) {
        printf("nothing in front of uncoupler\n");
        loop(1'000'000);
    }

    int32_t now_us = int32_t(time_us_32());
    int32_t detect_us = now_us;
    int32_t no_detect_us = 0;

    // in each direction, wait for one second with no detect
    const int32_t end_us = 1'000'000;

    int speed_mms = creep_mms;

    // creep back & forth in front of uncoupler sensor
    while (true) {

        DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(speed_mms));

        // get to sensor
        while (!Desktop::sensor_unc())
            loop();
        detect_us = int32_t(time_us_32());

        bool end_of_train;

        end_of_train = false;
        while (!end_of_train) {

            // look for gap or end
            while (Desktop::sensor_unc())
                loop();
            no_detect_us = int32_t(time_us_32());

            int32_t d1_us = no_detect_us - detect_us;
            printf("detect for %ld ms (%ld mm)\n", (d1_us + 500) / 1000, creep_mm(d1_us));

            // pass gap or go far enough to know it's the end
            while (!end_of_train) {
                if (Desktop::sensor_unc()) {
                    detect_us = int32_t(time_us_32());
                    break;
                }
                int32_t gap_us = int32_t(time_us_32()) - no_detect_us;
                end_of_train = (gap_us >= end_us);
            }

            if (!end_of_train) {
                int32_t d2_us = detect_us - no_detect_us;
                printf("gap for %ld ms (%ld mm)\n", (d2_us + 500) / 1000, creep_mm(d2_us));
            }

        } // while (!end_of_train)

        speed_mms = -speed_mms;

    } // while (true)

    return 0;
}


[[maybe_unused]]
static void uncoupler_sensor(bool active, intptr_t)
{
    static uint32_t last_us = UINT32_MAX; // last time this callback was called
    uint32_t now_us = time_us_32();
    printf("uncoupler sensor %s", active ? "active" : "inactive");
    if (last_us != UINT32_MAX)
        printf(" (+%lu ms)", (now_us - last_us + 500) / 1000);
    printf("\n");
    last_us = now_us;
}


static void loop(int32_t for_us)
{
    int32_t end_us = int32_t(time_us_32()) + for_us;
    while (end_us - int32_t(time_us_32()) >= 0) {
        tight_loop_contents();
        SysLed::loop();
    }
}


static void init()
{
    Desktop::init();

    DccApi::init(dcc_bit_gpio, dcc_pwr_gpio, dcc_adc_gpio, dcc_rcom_gpio,
                 dcc_rcom_uart);

    Status s;

    printf("reset loco ... ");
    while ((s = DccApi::cv_val_set(8, 8)) != Status::Ok) {
        printf("%s ... ", DccApi::status(s));
        loop(1'000'000);
    }
    printf("ok\n");

    loop(1'000'000);

    printf("create loco ... ");
    assert(DccApi::loco_create(loco_id) == Status::Ok);
    printf("ok\n");

    printf("track on ... ");
    assert(DccApi::track_set(true) == Status::Ok);
    printf("ok\n");

    loop(1'000'000); // wait for loco to boot up

    printf("read sn ... ");
    uint32_t sn;
    while ((s = Loco::read_sn(loco_id, sn)) != Status::Ok) {
        printf("%s ... ", DccApi::status(s));
        loop(1'000'000);
    }
    printf("%lu\n", sn);

    loco = Loco::find_loco(sn);
    assert(loco != nullptr);
    printf("loco: %s\n", loco->name);

    ops_cv_val_set(3, 0);
    ops_cv_val_set(4, 0);

} // init


static void ops_cv_val_set(int cv_num, int cv_val)
{
    if (cv_val < 0)
        return;

    printf("cv%d = %d ... ", cv_num, cv_val);
    while (true) {
        Status s = DccApi::loco_cv_val_set(loco_id, cv_num, cv_val);
        if (s == Status::Ok)
            break;
        printf("%s ... ", DccApi::status(s));
        loop(1'000'000);
    }
    printf("ok\n");

} // ops_cv_val_set
