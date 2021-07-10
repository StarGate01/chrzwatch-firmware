/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-19
 */

#include <mbed_stats.h>

#include "DisplayService.h"
#include "UserSettings.h"

#include "roboto_bold_48_minimal.h"
#include "roboto_bold_36_minimal.h"


Screen::Screen():
    _lcd(PIN_LCD_MOSI, NC, PIN_LCD_CLK, PIN_LCD_CS, 
        PIN_LCD_DC, PIN_LCD_RESET, LCD_SPI_FREQ),
    _display_guard(1),
    _flash(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_ACC_CS)
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

    if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_BLACK, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);

    switch(_state)
    {
        case ScreenState::STATE_CLOCK:
        {
            // Decode timestamp
            tm now;
            localtime_r(&_epochTime, &now);

            // Handle hour format
            int hour = now.tm_hour;
            uint8_t clock_indicator = hour > 12? 2 : 1; // PM : AM
            if (user_settings.time_format == 0) clock_indicator = 0;
            if (hour > 12 && user_settings.time_format == 1) hour -= 12;

            // Compute digits
            uint8_t clock_digits[4] = {
                hour / 10,
                hour % 10,
                now.tm_min / 10,
                now.tm_min % 10
            };

            // Re-layout if cache is invalid
            bool re_layout = _prev_state != _state
                || (_clock_indicator_cache > 0 && user_settings.time_format == 0)
                || (_clock_indicator_cache == 0 && user_settings.time_format == 1);
            if (re_layout) _lcd.fillFastScreen(LCD_COLOR_BLACK, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);

            // Draw digits
            for(uint8_t i = 0; i < 4; i++)
            {
                // Check cache
                if(re_layout
                    || _clock_digit_cache[i] != clock_digits[i]
                    || (i == 2 && _clock_digit_cache[0] != clock_digits[0]) // Minutes have to rendered again after hours
                    || (i == 3 && _clock_digit_cache[1] != clock_digits[1])) // Due to font overlap
                {
                    _lcd.drawFastBitmap(_clock_digit_pos[i][0], _clock_digit_pos[i][1],
                        roboto_bold_48_minimal + (roboto_bold_48_minimal_bs * clock_digits[i]), 
                        roboto_bold_48_minimal_w, roboto_bold_48_minimal_h, LCD_COLOR_WHITE, LCD_COLOR_BLACK, 
                        _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
                    
                    // Subsequent digits and indicator have to be redrawn
                    re_layout = true;
                }

                // Update cache
                _clock_digit_cache[i] = clock_digits[i];
            }

            // Draw indicator field
            if(re_layout || (clock_indicator != _clock_indicator_cache))
            {
                if(user_settings.time_format == 0)
                {
                    // Clear indicator
                    for(int i = 0; i < 2; i++)
                    {
                        _lcd.fillFastRect(_clock_indicator_pos[i][0], _clock_indicator_pos[i][1], 
                        roboto_bold_36_minimal_w, roboto_bold_36_minimal_h, LCD_COLOR_BLACK,
                        _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
                    }
                }
                else
                {
                    // Draw AM / PM
                    _lcd.drawFastBitmap(_clock_indicator_pos[0][0], _clock_indicator_pos[0][1],
                        roboto_bold_36_minimal + (roboto_bold_36_minimal_bs * clock_indicator), 
                        roboto_bold_36_minimal_w, roboto_bold_36_minimal_h, LCD_COLOR_WHITE, LCD_COLOR_BLACK, 
                        _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
                    _lcd.drawFastBitmap(_clock_indicator_pos[1][0], _clock_indicator_pos[1][1], roboto_bold_36_minimal, 
                        roboto_bold_36_minimal_w, roboto_bold_36_minimal_h, LCD_COLOR_WHITE, LCD_COLOR_BLACK, 
                        _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
                }
            }

            // Update cache
            _clock_indicator_cache = clock_indicator;

            break;
        }
        case ScreenState::STATE_HEART:
        {
            if(_prev_state != _state) 
            {
                _lcd.fillFastScreen(LCD_COLOR_RED, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);

                char buffer[1];
                _flash.read_raw(0x0, 1, buffer);
            }
            break;
        }
        case ScreenState::STATE_CADENCE:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_BLUE, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_STEPS:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_GREEN, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_DISTANCE:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_BLUE, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_SETTINGS:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_GREEN, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
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