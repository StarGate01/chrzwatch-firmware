/**
 * @file DisplayService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "DisplayService.h"


DisplayService::DisplayService():
    screen(Screen()),
    _vibration(PIN_VIBRATION),
    _vibration_trigger(1),
    _vibration_duration(200),
    _lcd_bl(PIN_LCD_BL),
    _lcd_pwr(PIN_LCD_PWR)
{
    _vibration_thread.start(callback(this, &DisplayService::threadVibration));
    setPower(true);
}

void DisplayService::setPower(bool on)
{
    if(on)
    {
        _lcd_pwr.write(1);
        _lcd_bl.write(1.0f);
    }
    else
    {
        _lcd_pwr.write(0);
        _lcd_bl.write(0.0f);
    }
    _is_on = on;
}

bool DisplayService::getPower()
{
    return _is_on;
}

void DisplayService::vibrate(uint16_t duration)
{
    _vibration_duration = duration;
    _vibration_trigger.release();
}

void DisplayService::render()
{
    screen.render();
}

void DisplayService::threadVibration()
{
    while(true)
    {
        _vibration_trigger.acquire();
#       if defined(PIN_VIBRATION_INVERT)
            _vibration = 0;
#       else
            _vibration = 1;
#       endif
        ThisThread::sleep_for(_vibration_duration);
#       if defined(PIN_VIBRATION_INVERT)
            _vibration = 1;
#       else
            _vibration = 0;
#       endif
    }
}