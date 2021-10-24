/**
 * @file Screen.h
 * @author Christoph Honal
 * @brief Defines a class to hold the state of the screen
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef SCREEN_H
#define SCREEN_H

#define MBED_CONF_RTOS_PRESENT 1

#include <mbed.h>
#include <events/mbed_events.h>
#include <rtos.h>

#include "Adafruit_ST7735_Mini.h"
// #include "GT24L24A2Y.h"

#include "HardwareConfiguration.h"
#include "fonts.h"
#include "icons.h"


#define LCD_BUFFER_SIZE     400  //!< LCD buffer in bytes

#define LCD_COLOR_BLACK     0x0000
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_RED       0x001F
#define LCD_COLOR_GREEN     0x7E00
#define LCD_COLOR_BLUE      0xF800
#define LCD_COLOR_YELLOW    0x7E1F
#define LCD_COLOR_GOLD      0x2BDB
#define LCD_COLOR_FROG      0x5E8B
#define LCD_COLOR_SKY       0xF8E7

// Forward decalarations
class DisplayService;


/**
 * @brief Holds the state of the display screen and talks to the LCD controller
 * 
 */
class Screen
{

    public:
        /**
         * @brief State possibilities of the screen state machine
         * 
         */
        enum ScreenState : int
        {
            STATE_CLOCK = 0, //!< Home screen, shows clock
            STATE_HEART, //!< Heartrate screen
            STATE_CADENCE, //!< Step cadence screen
            STATE_STEPS, //!< Amount of steps screen
            STATE_DISTANCE, //!< Distance walked screen
            STATE_SETTINGS, //!< Settings screen
            STATE_INFO, //!< Info screen
            STATE_LOOP //!< Back to home
        };

        /**
         * @brief Construct a new Screen Model object
         *  
         */
        Screen();

        /**
         * @brief Renders the model to a LCD
         */
        void render();

        /**
         * @brief Sets the screen state
         * 
         * @param state The new state
         */
        void setState(enum ScreenState state);

        /**
         * @brief Get the state
         * 
         * @return enum ScreenState current screen state
         */
        enum ScreenState getState();


    protected:
        Adafruit_ST7735_Mini _lcd; //!< LCD output
        char _lcd_bitmap_buffer[LCD_BUFFER_SIZE]; //!< Buffer for fast bitmap drawing
        // GT24L24A2Y _flash; //!< Flash font interface

        enum ScreenState _state = ScreenState::STATE_CLOCK; //!< Stores screen state
        enum ScreenState _prev_state = ScreenState::STATE_LOOP; //!< Previous screen state
        Semaphore _display_guard; //!< Serialize display bus access

        uint8_t _clock_digit_cache[4]; //!< Cache for the clock digits
        uint8_t _clock_indicator_cache; //!< Cache for the clock format, 0 = 24h (empty), 1 = AM, 2 = PM
        const uint8_t _clock_digit_pos[4][2] = { 
            {3, 5}, {39, 5}, {3, 60}, {39, 60} }; //!< Pixel positions of the clock digits
        const uint8_t _clock_indicator_pos[2][2] = { 
            {6, 115}, {44, 115} }; //!< Pixel positions of the clock AM/PM indicator
        const uint8_t _clock_24_y_offset = 17; //!< How many pixels to move digits down in 24h mode
        const uint8_t _icon_pos[2] = { 20, 12 }; //!< Icon position
        const uint8_t _icon_value_height = 57; //!< Value below icon y offset
        const uint8_t _icon_value_margin[3] = { 30, 20, 9 }; //!< Space from left edge
        const uint8_t _icon_value_squish = 3; //!< Squish letters together
        
        mbed_stats_cpu_t _cpu_stats; //!< Buffer for cpu stats
        time_t _epochTime = 0; //!< Current time
        float _batteryPercent = 0; //!< Battery remaining in percent
        uint8_t _heartrate = 0; //!< Heartrate
        bool _batteryCharging = false; //!< Battery charging state
        bool _bleStatus = false; //!< Bluetooth status
        bool _bleEncStatus = false; //!< Bluetooth Enc status
        char _bleAddress[19]; //< BLE address string
        uint8_t _stepsCadence = 0; //!< Steps cadence
        uint32_t _stepsTotal = 0; //!< Steps total

        /**
         * @brief Renders a row of text with 24pt font
         * 
         * @param row Row 0 - 2
         * @param indices The font glyphs to render
         * @param count How many glyphs to draw
         */
        void renderGlyphs(uint8_t row, const uint8_t indices[], uint8_t count);

        /**
         * @brief Converts a number to glyph values
         * 
         * @param value The number to convert
         * @param indices The glyphs
         * @param count The glyph count
         * @param leading_zeros Add leading zeros
         */
        void valueToGlyphs(uint16_t value, uint8_t indices[3], uint8_t& count, bool leading_zeros = false);

        friend class DisplayService; // Display may access internal screen variables
};

#endif