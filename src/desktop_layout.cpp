
#include "sensor2.h"
#include "sensor.h"
#include "turnout.h"
#include "desktop_layout.h"

#include "config.h"


Sensor2 sensor2[sensor2_max] = {
    Sensor2(s0_gpio),
    Sensor2(s1_gpio),
    Sensor2(s2_gpio),
    Sensor2(s3_gpio),
};


Sensor sensor[sensor_max] = {
    Sensor(s4_gpio),
};


Turnout turnout[turnout_max] = {
    Turnout(t0a_gpio, t0b_gpio),
    Turnout(t1a_gpio, t1b_gpio),
};


// sensor in house
Sensor2& sensor_home()
{
    return sensor2[0];
}


// sensor at uncoupler
Sensor& sensor_unc()
{
    return sensor[0];
}


// sensor at end of spur
Sensor2& sensor_spur(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        return sensor2[1];
    else if (spur_num == 2)
        return sensor2[2];
    else // (spur_num == 3)
        return sensor2[3];
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


// distance from uncoupler sensor to end of spur
int unc_to_spur_mm(int spur_num)
{
    assert(spur_num == 1 || spur_num == 2 || spur_num == 3);

    if (spur_num == 1)
        return s4_s1_mm;
    else if (spur_num == 2)
        return s4_s2_mm;
    else // (spur_num == 3)
        return s4_s3_mm;
}
