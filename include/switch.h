#pragma once

#include "pico/stdlib.h"
#include "hardware/gpio.h"

class Switch
{
public:

    Switch(int gpio) :
        _gpio(gpio)
    {
        gpio_init(_gpio);
        gpio_pull_up(_gpio);
    }

    ~Switch()
    {
        gpio_init(_gpio);
        _gpio = -1;
    }

    // return true if switch closed (pin grounded)
    operator bool() const
    {
        return !gpio_get(_gpio);
    }

private:

    int _gpio;

}; // class Switch
