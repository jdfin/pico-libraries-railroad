
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h" // sleep_ms
// misc
#include "misc/sys_led.h"
// dcc
#include "dcc/dcc_api.h"
using Status = DccApi::Status;
// railroad
#include "railroad/config.h"
#include "railroad/desktop_layout.h"
#include "railroad/locos.h"

#if 0

Home:
Start with loco + car with loco nose in front of uncoupler.
sensor[0] detects far wall of house.
Creep forward until sensor[0] changes by more than a threshold.
Then keep going forward until close and stop.
Creep backward until sensor[0] sees only the wall.

Spur:
Start with rear end of train left of uncoupler.
Line turnouts for spur.
Creep backward until close, noting first detect and last non-detect.
Creep forward, noting first non-detect and last detect.
Forward to uncoupler, then more until uncoupler non-detect for time.
#endif

static constexpr int loco_id = 3;
static const Loco *loco = nullptr;

static void loop(int32_t for_us = 0);
static void init();
static void ops_cv_val_set(int cv_num, int cv_val);
static void test_home();
static void test_spur(int spur_num);
static void show_spur(int spur_num);

static const int creep_mms = 20;
static const int stop = 0;


int main()
{
    stdio_init_all();
    SysLed::init();
    SysLed::pattern(50, 950);

    while (!stdio_usb_connected())
        loop();
    sleep_ms(10); // small delay needed or we lose the first prints

    SysLed::off();

    printf("\n");
    printf("sensor2_test2\n");
    printf("\n");

    init();

    // start with loco nose in front of uncoupler
    while (!Desktop::sensor_unc()) {
        printf("nothing in front of uncoupler\n");
        loop(1'000'000);
    }

    while (true) {
        //test_home();
        for (int spur_num = 1; spur_num <= 3; spur_num++)
            test_spur(spur_num);
        loop(2'000'000);
    }

    return 0;
}


static void loop(int32_t for_us)
{
    int32_t end_us = int32_t(time_us_32()) + for_us;
    while (end_us - int32_t(time_us_32()) >= 0) {
        tight_loop_contents();
        SysLed::loop();
    }
} // loop


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


[[maybe_unused]]
static void test_home()
{
    int initial_dist_mm = 0;
    const int initial_avg_cnt = 1;

    for (int i = 0; i < initial_avg_cnt; i++) {
        initial_dist_mm += Desktop::sensor_home().dist_mm();
        loop(50'000);
    }
    initial_dist_mm = (initial_dist_mm + initial_avg_cnt / 2) / initial_avg_cnt;
    printf("test_home: initial distance %d mm\n", initial_dist_mm);

    // This average varies by a mm or two if run repeatedly.
    // Actual distance to far wall is ~220mm.
    // Read 203mm - 205mm with light on and roof off
    // Read 210mm - 211mm with light on roof on
    // Read 209mm - 211mm with light off roof on
    // Read 204mm - 206mm with light off roof off
    // So reading is ~10% off, light doesn't matter, roof changes reading 5mm =  2.5%
    // Without averaging, the variation is ~5mm instead of ~2mm.
}


[[maybe_unused]]
static void test_spur(int spur_num)
{
    printf("spur %d\n", spur_num);

    Desktop::line_turnout_0(spur_num);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));
    loop(1'000'000);
    Desktop::line_turnout_1(spur_num);

    printf("approach: ");

    int approach_first_detect_mm = Sensor2::infinity;
    while (approach_first_detect_mm == Sensor2::infinity) {
        loop();
        approach_first_detect_mm = Desktop::sensor_spur(spur_num).dist_mm();
    }
    printf("first detect at %d mm; ", approach_first_detect_mm);

    int approach_continuous_detect_mm = approach_first_detect_mm;
    int new_mm = Sensor2::infinity;
    while (new_mm > 50) {
        new_mm = Desktop::sensor_spur(spur_num).dist_mm();
        if (new_mm == Sensor2::infinity || approach_continuous_detect_mm == Sensor2::infinity)
            approach_continuous_detect_mm = new_mm;
        loop();
    }
    printf("continuous at %d mm\n", approach_continuous_detect_mm);

    DccApi::loco_speed_set(loco_id, stop);
    loop(2'000'000);

    printf("depart: ");

    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));

    int depart_continuous_detect_mm = 0;
    new_mm = Desktop::sensor_spur(spur_num).dist_mm();
    while (new_mm != Sensor2::infinity) {
        depart_continuous_detect_mm = new_mm;
        loop();
        new_mm = Desktop::sensor_spur(spur_num).dist_mm();
    }
    printf("continuous until %d mm; ", depart_continuous_detect_mm);

    int depart_last_detect_mm = depart_continuous_detect_mm;
    // save last detect over next five seconds
    int32_t end_us = int32_t(time_us_32()) + 5'000'000;
    while (end_us - int32_t(time_us_32()) >= 0) {
        new_mm = Desktop::sensor_spur(spur_num).dist_mm();
        if (new_mm != Sensor2::infinity)
            depart_last_detect_mm = new_mm;
        loop();
    }
    printf("last detect at %d mm\n", depart_last_detect_mm);

    // continue to uncoupler

    while (!Desktop::sensor_unc())
        loop();

    // and continue until we don't see uncoupler for two seconds
    end_us = int32_t(time_us_32()) + 2'000'000;
    while (end_us - int32_t(time_us_32()) >= 0) {
        if (Desktop::sensor_unc())
            end_us = int32_t(time_us_32()) + 2'000'000;
    }

    DccApi::loco_speed_set(loco_id, stop);
    loop(2'000'000);

} // test_spur()


[[maybe_unused]]
static void show_spur(int spur_num)
{
    printf("spur %d\n", spur_num);

    Desktop::line_turnout_0(spur_num);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(-creep_mms));
    loop(1'000'000);
    Desktop::line_turnout_1(spur_num);

    int new_mm;

    printf("----");
    do {
        printf("\b\b\b\b");
        new_mm = Desktop::sensor_spur(spur_num).dist_mm();
        if (new_mm == Sensor2::infinity)
            printf("----");
        else
            printf("%4d", new_mm);
        loop(10'000);
    } while (new_mm > 50);

    DccApi::loco_speed_set(loco_id, stop);
    loop(2'000'000);
    DccApi::loco_speed_set(loco_id, loco->mms_to_dcc(creep_mms));

    while (!Desktop::sensor_unc()) {
        printf("\b\b\b\b");
        new_mm = Desktop::sensor_spur(spur_num).dist_mm();
        if (new_mm == Sensor2::infinity)
            printf("----");
        else
            printf("%4d", new_mm);
        loop(10'000);
    }

    printf("\n");

    // and continue until we don't see uncoupler for two seconds
    int32_t end_us = int32_t(time_us_32()) + 2'000'000;
    while (end_us - int32_t(time_us_32()) >= 0) {
        if (Desktop::sensor_unc())
            end_us = int32_t(time_us_32()) + 2'000'000;
    }

    DccApi::loco_speed_set(loco_id, stop);
    loop(2'000'000);

} // test_spur()
