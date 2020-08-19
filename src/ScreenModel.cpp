/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-19
 */

#include "DisplayService.h"


ScreenModel::ScreenModel():
    epochTime(0),
    batteryValue(0),
    batteryCharging(true),
    batteryCharging2(true)
{
}

void ScreenModel::render(Adafruit_ST7735_Mini &lcd)
{
    char time_buff[20];
    strftime(time_buff, 20, "%H:%M:%S\n%d.%m.%Y", localtime(&epochTime));
    lcd.setCursor(0, 30);
    lcd.printf(time_buff);
    lcd.setCursor(0, 70);
    lcd.printf("%u%%", batteryValue);
    lcd.setCursor(0, 100);
    lcd.printf(batteryCharging? "Yes" : "No ");
    lcd.setCursor(0, 110);
    lcd.printf(batteryCharging2? "Yes" : "No ");
}