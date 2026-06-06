#pragma once

// pico
#include "pico/stdlib.h"
// misc
#include "misc/i2c_dev.h"
// oled
#include "oled/chicago_12.h"
#include "oled/font.h"
#include "oled/oled.h"
// railroad
#include "railroad/config.h"


class Display
{

public:

    Display() :
        _i2c(disp_i2c_dev, disp_scl_gpio, disp_sda_gpio, Oled::baud),
        _oled(_i2c, disp_i2c_adrs, &chicago_12)
    {
    }

    void init()
    {
        _oled.init();
        clear();
        flush();
        wait();
        _oled.on();
    }

    void clear(int col = 0, int row = 0, int wid = width(), int hgt = height())
    {
        _oled.fill(col, row, wid, hgt, Oled::Color::Background);
    }

    using HAlign = Oled::HAlign;
    using VAlign = Oled::VAlign;

    void set_align(HAlign ha, VAlign va)
    {
        _oled.set_align(ha, va);
    }

    void putc(int col, int row, char ch)
    {
        _oled.putc(col, row, ch);
    }

    void puts(int col, int row, const char *str)
    {
        _oled.puts(col, row, str);
    }

    void show(const char *str)
    {
        clear();
        set_align(HAlign::Center, VAlign::Center);
        puts(width() / 2, height() / 2, str);
        flush();
    }

    void flush()
    {
        _oled.flush();
    }

    // Wait for write over i2c to display to finish
    void wait()
    {
        while (_i2c.write_status() == I2cDev::Status::Busy)
            tight_loop_contents();
    }

    static constexpr int width()
    {
        return Oled::cols;
    }

    static constexpr int height()
    {
        return Oled::rows;
    }

private:

    I2cDev _i2c;
    Oled _oled;
};
