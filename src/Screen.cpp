/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in Screen.h
 * @version 0.1
 * @date 2020-08-19
 */

#include <mbed_stats.h>

#include "Screen.h"
#include "UserSettings.h"
#include "CoreService.h"

#include "roboto_bold_48_minimal.h"
#include "roboto_bold_36_minimal.h"


Screen::Screen():
    _lcd(PIN_LCD_MOSI, NC, PIN_LCD_CLK, PIN_LCD_CS, 
        PIN_LCD_DC, PIN_LCD_RESET, LCD_SPI_FREQ),
    _display_guard(1)
    // _flash(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_ACC_CS)
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
            bool re_layout = (_prev_state != _state)
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
                    _lcd.drawFastBitmap(_clock_digit_pos[i][0], _clock_digit_pos[i][1] + 
                            ((user_settings.time_format == 0)? _clock_24_y_offset : 0),
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
                if(user_settings.time_format == 1)
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
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_RED, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_CADENCE:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_GOLD, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_STEPS:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_FROG, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_DISTANCE:
        {
            if(_prev_state != _state) _lcd.fillFastScreen(LCD_COLOR_BLUE, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            break;
        }
        case ScreenState::STATE_SETTINGS:
        {
            // Display settings
            _lcd.fillFastScreen(LCD_COLOR_BLACK, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            _lcd.setCursor(0, 8);
            _lcd.setTextColor(LCD_COLOR_YELLOW);
            _lcd.printf("Settings\n\n");
            _lcd.setTextColor(LCD_COLOR_WHITE);
            _lcd.printf("[General]\n");
            _lcd.printf("Time format\n %s\n", user_settings.time_format == 1? "12h":"24h");
            _lcd.printf("Btn vibrate\n %s\n", user_settings.button_feedback? "Yes":"No");
            _lcd.printf("\n[Sensor]\n");
            _lcd.printf("Step length\n %u cm\n", user_settings.sensor.step_length);
            _lcd.printf("Motion dur\n %u 1/50s\n", user_settings.sensor.motion_duration);
            _lcd.printf("Motion thresh\n %u 1/16g\n", user_settings.sensor.motion_threshold);
            _lcd.printf("Run cadence\n %u spm", user_settings.sensor.cadence_running_thresh);
            break;
        }    
        case ScreenState::STATE_INFO:
        {
            // Gather CPU stats
            mbed_stats_cpu_get(&_cpu_stats);

            // Display info
            _lcd.fillFastScreen(LCD_COLOR_BLACK, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
            _lcd.setCursor(0, 8);
            _lcd.setTextColor(LCD_COLOR_YELLOW);
            _lcd.printf("System Info\n\n");
            _lcd.setTextColor(LCD_COLOR_WHITE);
            _lcd.printf("[Bluetooth]\n");
            _lcd.printf("Address\n %s\n", _bleAddress);
            _lcd.printf("Connected\n %s\n", _bleStatus? "Yes":"No");
            _lcd.printf("Bonded\n %s\n", _bleEncStatus? "Yes":"No");
            _lcd.printf("\n[CPU]\n");
            _lcd.printf("Uptime\n %llu s\n", _cpu_stats.uptime / 1000000);
            _lcd.printf("Sleep\n %llu s\n", _cpu_stats.sleep_time / 1000000);
            _lcd.printf("Deep sleep\n %llu s", _cpu_stats.deep_sleep_time / 1000000);
            break;
        }    
        default: break;
    }

    // Draw battery bar
    uint8_t bar_len = min((uint8_t)round(_batteryPercent * 0.8f), (uint8_t)80);
    if(bar_len < 80) _lcd.fillFastRect(bar_len, 0, 80 - bar_len, 4, _batteryCharging? LCD_COLOR_BLUE : LCD_COLOR_BLACK, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);
    uint16_t bar_color = LCD_COLOR_FROG;
    if(_batteryPercent < 25.f) bar_color = LCD_COLOR_RED;
    else if(_batteryPercent < 50.f) bar_color = LCD_COLOR_GOLD;
    if(bar_len > 0) _lcd.fillFastRect(0, 0, bar_len, 4, bar_color, _lcd_bitmap_buffer, LCD_BUFFER_SIZE);

    // Update state
    _prev_state = _state;

    _display_guard.release();
}