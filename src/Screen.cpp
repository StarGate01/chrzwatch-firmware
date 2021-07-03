/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-19
 */

#include "DisplayService.h"
#include "mbed_stats.h"


Screen::Screen():
    epochTime(0),
    batteryPercent(0),
    batteryRaw(0),
    batteryCharging(true),
    bleStatus(false),
    bleEncStatus(false),
    lcd(PIN_LCD_SDA, NC, PIN_LCD_SCL, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RESET),
    _display_guard(1)
{
    _display_guard.acquire();

    // Init display controller
    lcd.initR(INITR_MINI160x80, LCD_COLSHIFT, LCD_ROWSHIFT);
#   if defined(LCD_INVERT)
        lcd.invertDisplay(true);
#   endif
    lcd.setRotation(2);
    lcd.setCursor(0, 0);
    lcd.setTextSize(1);
    lcd.setTextWrap(false);
    lcd.fillScreen(ST7735_BLACK);
    lcd.setTextColor(ST7735_CYAN, ST7735_BLACK);
    lcd.printf("CHRZ Watch\n\nTime:\n\n\n\n\nBattery:\n\n\n\nCharging:\n");

    _display_guard.release();
}

void Screen::render()
{
    bool sem = _display_guard.try_acquire();
    if(!sem) return; // Cancel if a rendering is already in progress

    // Decode timestamp to string
    char time_buff[20];
    strftime(time_buff, 20, "%H:%M:%S\n%d.%m.%Y", localtime(&epochTime));

    lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    lcd.setCursor(0, 30);
    lcd.printf(time_buff);
    lcd.setCursor(0, 70);
    lcd.printf("%u%% (%f)", batteryPercent, batteryRaw);
    lcd.setCursor(0, 100);
    lcd.printf(batteryCharging? "Yes, HR: %u  " : "No, HR: %u  ", heartrate);
    lcd.setCursor(0, 115);
    if(bleStatus)
    {
        lcd.setTextColor(ST7735_GREEN, ST7735_BLACK);
        if(bleEncStatus)
        {
            lcd.printf("BLE: [x, E]");
        }
        else
        {
            lcd.printf("BLE: [x, N]");
        }
    }
    else
    {
        lcd.setTextColor(ST7735_RED, ST7735_BLACK);
        lcd.printf("BLE: [ ]   ");
    }

    lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    lcd.setCursor(0, 130);
    lcd.printf("ST: %u\nSC: %u", stepsTotal, stepsCadence);

    // mbed_stats_cpu_t stats;
    // mbed_stats_cpu_get(&stats);
    // lcd.printf("UP: %llu\nST: %llu\nDS: %llu", 
    // stats.uptime / 1000, stats.sleep_time / 1000, stats.deep_sleep_time / 1000);

    _display_guard.release();
}