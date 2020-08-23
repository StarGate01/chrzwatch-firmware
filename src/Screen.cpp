/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-19
 */

#include "DisplayService.h"


Screen::Screen():
    epochTime(0),
    batteryPercent(0),
    batteryRaw(0),
    batteryCharging(true),
    bleStatus(false),
    _lcd(PIN_LCD_SDA, NC, PIN_LCD_SCL, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RESET),
    _display_guard(1)
{
    _display_guard.acquire();

    // Init display controller
    _lcd.initR(INITR_MINI160x80, LCD_COLSHIFT, LCD_ROWSHIFT);
#   if defined(LCD_INVERT)
        _lcd.invertDisplay(true);
#   endif
    _lcd.setRotation(2);
    _lcd.setCursor(0, 0);
    _lcd.setTextSize(1);
    _lcd.setTextWrap(false);
    _lcd.fillScreen(ST7735_BLACK);
    _lcd.setTextColor(ST7735_CYAN, ST7735_BLACK);
    _lcd.printf("CHRZ Watch\n\nTime:\n\n\n\n\nBattery:\n\n\n\nCharging:\n");

    _display_guard.release();
}

void Screen::render()
{
    bool sem = _display_guard.try_acquire();
    if(!sem) return; // Cancel if a rendering is already in progress

    // Decode timestamp to string
    char time_buff[20];
    strftime(time_buff, 20, "%H:%M:%S\n%d.%m.%Y", localtime(&epochTime));

    _lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    _lcd.setCursor(0, 30);
    _lcd.printf(time_buff);
    _lcd.setCursor(0, 70);
    _lcd.printf("%u%% (%ul)", batteryPercent, batteryRaw);
    _lcd.setCursor(0, 100);
    _lcd.printf(batteryCharging? "Yes" : "No ");
    _lcd.setCursor(0, 130);
    if(bleStatus)
    {
        _lcd.setTextColor(ST7735_GREEN, ST7735_BLACK);
        _lcd.printf("BLE: [x]");
    }
    else
    {
        _lcd.setTextColor(ST7735_RED, ST7735_BLACK);
        _lcd.printf("BLE: [ ]");
    }

    _display_guard.release();
}