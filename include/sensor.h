#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"


class Sensor
{

public:

    Sensor(int gpio) :
        _gpio(gpio)
    {
        gpio_init(gpio);
        gpio_set_dir(_gpio, GPIO_IN);
    }

    void init() const
    {
    }

    operator bool() const
    {
        return !gpio_get(_gpio);
    }

private:

    int _gpio;
};
