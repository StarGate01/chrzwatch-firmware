/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-19
 */

#include "DisplayService.h"
#include "mbed_stats.h"

#include <roboto_bold_48_minimal.h>


Screen::Screen():
    epochTime(0),
    batteryPercent(0),
    batteryRaw(0),
    batteryCharging(true),
    bleStatus(false),
    bleEncStatus(false),
    lcd(PIN_LCD_SDA, NC, PIN_LCD_SCL, PIN_LCD_CS, 
        PIN_LCD_DC, PIN_LCD_RESET, LCD_SPI_FREQ),
    _display_guard(1)
    // _font_reader(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_FONT_CS)
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
    // lcd.setTextColor(ST7735_CYAN, ST7735_BLACK);
    // lcd.fillScreen(ST7735_BLACK);
    // lcd.printf("CHRZ Watch\n\nTime:\n\n\n\n\nBattery:\n\n\n\nCharging:\n");

    // struct font_layout_t font = FONT(CLOCK4_32);
    // char buffer[FONT_CLOCK4_32_BSIZE];
    // uint16_t actual_width;
    // test = _font_reader.read(font, 0, buffer, &actual_width);
    // test2 = actual_width;
    // lcd.drawBitmap(0, 0, (uint8_t*)buffer, 22, 32, ST7735_CYAN, true, 2);

    _display_guard.release();
}

void Screen::setState(enum Screen::ScreenState state)
{
    _prev_state = _state;
    _state = state;
}

enum Screen::ScreenState Screen::getState()
{
    return _state;
}

void Screen::render()
{
    _display_guard.acquire(); // Wait for lcd to be available

    if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_BLACK, lcd_bitmap_buffer, LCD_BUFFER_SIZE);

    switch(_state)
    {
        case ScreenState::STATE_CLOCK:
        {
            // Decode timestamp
            tm now;
            localtime_r(&epochTime, &now);

            // Compute digits
            uint8_t clock_digits[4] = {
                now.tm_min / 10,
                now.tm_min % 10,
                now.tm_sec / 10,
                now.tm_sec % 10
            };

            for(uint8_t i = 0; i < 4; i++)
            {
                //Check cache
                if(_prev_state != _state || _clock_digit_cache[i] != clock_digits[i])
                {
                    lcd.drawFastBitmap(_clock_digit_pos[i][0], _clock_digit_pos[i][1],
                        roboto_bold_48_minimal + (roboto_bold_48_minimal_bs * clock_digits[i]), 
                        roboto_bold_48_minimal_w, roboto_bold_48_minimal_h, LCD_COLOR_WHITE, LCD_COLOR_RED, 
                        lcd_bitmap_buffer, LCD_BUFFER_SIZE, true);
                }

                // Update cache
                _clock_digit_cache[i] = clock_digits[i];
            }
            break;
        }
        case ScreenState::STATE_HEART:
        {
            if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_RED, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_CADENCE:
        {
            if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_BLUE, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_STEPS:
        {
            if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_GREEN, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_DISTANCE:
        {
            if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_BLUE, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_SETTINGS:
        {
            if(_prev_state != _state) lcd.fillFastScreen(LCD_COLOR_GREEN, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }    
        default: break;
    }

    // Update state
    _prev_state = _state;

    // // Decode timestamp to string
    // char time_buff[20];
    // strftime(time_buff, 20, "%H:%M:%S\n%d.%m.%Y", localtime(&epochTime));

    // lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    // lcd.setCursor(0, 30);
    // lcd.printf(time_buff);
    // lcd.setCursor(0, 70);
    // lcd.printf("%u%% (%f)", batteryPercent, batteryRaw);
    // lcd.setCursor(0, 100);
    // lcd.printf(batteryCharging? "Yes, HR: %u  " : "No, HR: %u  ", heartrate);
    // lcd.setCursor(0, 115);
    // if(bleStatus)
    // {
    //     lcd.setTextColor(ST7735_GREEN, ST7735_BLACK);
    //     if(bleEncStatus)
    //     {
    //         lcd.printf("BLE: [x, E]");
    //     }
    //     else
    //     {
    //         lcd.printf("BLE: [x, N]");
    //     }
    // }
    // else
    // {
    //     lcd.setTextColor(ST7735_RED, ST7735_BLACK);
    //     lcd.printf("BLE: [ ]   ");
    // }

    // lcd.setTextColor(ST7735_WHITE, ST7735_BLACK);
    // lcd.setCursor(0, 130);
    // lcd.printf("ST: %u\nSC: %u", stepsTotal, stepsCadence);

    // mbed_stats_cpu_t stats;
    // mbed_stats_cpu_get(&stats);
    // lcd.printf("UP: %llu\nST: %llu\nDS: %llu", 
    // stats.uptime / 1000, stats.sleep_time / 1000, stats.deep_sleep_time / 1000);

    _display_guard.release();
}