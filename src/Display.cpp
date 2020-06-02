/**
 * @file DisplayService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "DisplayService.h"


DisplayService::DisplayService():
    _vibration(PIN_VIBRATION),
    _vibration_trigger(1),
    _vibration_duration(200),
    _lcd(NC, PIN_LCD_SDA, PIN_LCD_SCL, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RESET, ST7735_TFTWIDTH_80, ST7735_TFTHEIGHT_160),
    _lcd_bl(PIN_LCD_BL)
{
    _vibration_thread.start(callback(this, &DisplayService::threadVibration));

    _lcd_bl.write(1.0f);
    _lcd.initR(INITR_MINI160x80);
    _lcd.fillScreen(ST7735_BLACK);
    _lcd.setRotation(2);
    _lcd.setCursor(0, 0);
    _lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    _lcd.setTextSize(2);
    _lcd.setTextWrap(false);
    _lcd.printf("Hello World\n");
}

void DisplayService::vibrate(uint16_t duration)
{
    _vibration_duration = duration;
    _vibration_trigger.release();
}

void DisplayService::render()
{
    _lcd.printf("Hello Render\n");
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