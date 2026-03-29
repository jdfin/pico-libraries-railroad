
#include "sensor.h"
#include "turnout.h"
#include "desktop_layout.h"

#include "config.h"


Sensor sensor[sensor_max] = {
    Sensor(s0_gpio), Sensor(s1_gpio), Sensor(s2_gpio), Sensor(s3_gpio),
    Sensor(s4_gpio), Sensor(s5_gpio), Sensor(s6_gpio), Sensor(s7_gpio),
};


Turnout turnout[turnout_max] = {
    Turnout(t0a_gpio, t0b_gpio),
    Turnout(t1a_gpio, t1b_gpio),
};


// sensor in house
Sensor& sensor_home()
{
    return sensor[0];
}


// sensor at uncoupler
Sensor& sensor_unc()
{
    return sensor[1];
}


// sensor 50 mm from end of spur
Sensor& sensor_50(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        return sensor[2];
    else if (spur_num == 2)
        return sensor[4];
    else // (spur_num == 3)
        return sensor[6];
}


// sensor 100 mm from end of spur
Sensor& sensor_100(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        return sensor[3];
    else if (spur_num == 2)
        return sensor[5];
    else // (spur_num == 3)
        return sensor[7];
}


void line_turnout_0(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        turnout[0].set(true);  // straight
    else                       // 2 or 3
        turnout[0].set(false); // diverge
}


void line_turnout_1(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        ; // nothing to do
    else if (spur_num == 2)
        turnout[1].set(false); // diverge
    else if (spur_num == 3)
        turnout[1].set(true); // straight
}


// distance from uncoupler sensor to 110 mm sensor on spur
int unc_to_spur_mm(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        return s1_s3_mm;
    else if (spur_num == 2)
        return s1_s5_mm;
    else // (spur_num == 3)
        return s1_s7_mm;
}
