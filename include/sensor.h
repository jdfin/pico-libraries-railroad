#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"
//
#include "gpio_extra.h"


class Sensor
{

public:

    Sensor(int gpio) :
        _gpio(gpio)
    {
        gpio_init(gpio);
        gpio_set_dir(_gpio, GPIO_IN);
    }

    void init()
    {
        gpiox_set_callback(_gpio, irq_handler, intptr_t(this));
        // irq is enabled only when callback is set
    }

    operator bool() const
    {
        return !gpio_get(_gpio);
    }

    void set_callback(void (*cb)(bool active, intptr_t arg), intptr_t arg)
    {
        gpio_set_irq_enabled(_gpio, GPIOX_IRQ_EDGE_ANY, false);
        _cb = cb;
        _cb_arg = arg;
        if (_cb != nullptr)
            gpio_set_irq_enabled(_gpio, GPIOX_IRQ_EDGE_ANY, true);
    }

private:

    int _gpio;

    // callback for changes
    void (*_cb)(bool active, intptr_t arg);
    intptr_t _cb_arg;

    // There is one static handler for all sensors. Each sensor attaches it
    // as the callback for its pin with a different arg to identify itself.
    static void irq_handler(uint gpio, uint32_t events, intptr_t arg)
    {
        Sensor *sensor = (Sensor *)arg;
        assert(int(gpio) == sensor->_gpio);
        assert(sensor->_cb != nullptr);
        // sensor is active (detect) low
        if (events & GPIO_IRQ_EDGE_RISE)
            sensor->_cb(false, sensor->_cb_arg); // nothing detected
        else if (events & GPIO_IRQ_EDGE_FALL)
            sensor->_cb(true, sensor->_cb_arg); // something detected
        else
            assert(false);
    }
};
