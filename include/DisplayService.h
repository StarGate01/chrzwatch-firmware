/**
 * @file DisplayService.h
 * @author Christoph Honal
 * @brief Defines a class to interact with the LCD display and other actors
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define MBED_CONF_RTOS_PRESENT 1

#include <mbed.h>
#include <events/mbed_events.h>
#include <rtos.h>

#include <Adafruit_ST7735_Mini.h>
// #include <GT24L24A2Y.h>

#include "HardwareConfiguration.h"
#include "CurrentTimeService.h"


#define LCD_TIMEOUT         5000 //!< LCD timeout in ms
#define LCD_BUFFER_SIZE     320  //!< LCD buffer in bytes
#define VIBRATION_GRACE_IN  10   //!< Sensor shutoff before vibration
#define VIBRATION_GRACE_OUT 750  //!< Sensor restart grace after vibration

#define LCD_COLOR_BLACK     0x0000
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_RED       0x001F
#define LCD_COLOR_GREEN     0x7E00
#define LCD_COLOR_BLUE      0xF800
#define LCD_COLOR_YELLOW    0x7E1F
#define LCD_COLOR_GOLD      0x67F0
#define LCD_COLOR_LIGHTRED  0x38FF

// Forward decalarations
class SensorService;


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
            STATE_SETTINGS, //!< Info and settings screen
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


        Adafruit_ST7735_Mini lcd; //!< LCD output
        char lcd_bitmap_buffer[LCD_BUFFER_SIZE]; //!< Buffer for fast bitmap drawing
        time_t epochTime; //!< Current time
        uint8_t batteryPercent; //!< Battery remaining in percent
        uint8_t heartrate; //!< Heartrate
        float batteryRaw; //!< Battery remaining raw (volts)
        bool batteryCharging; //!< Battery charging state
        bool bleStatus; //!< Bluetooth status
        bool bleEncStatus; //!< Bluetooth Enc status
        uint8_t stepsCadence; //!< Steps cadence
        uint32_t stepsTotal; //!< Steps total

    protected:
        enum ScreenState _state = ScreenState::STATE_CLOCK; //!< Stores screen state
        enum ScreenState _prev_state = ScreenState::STATE_LOOP; //!< Previous screen state
        Semaphore _display_guard; //!< Serialize display bus access
        // GT24L24A2Y _font_reader; //!< Font ROM interface
        uint8_t _clock_digit_cache[4]; //!< Cache for the clock digits
        uint8_t _clock_indicator_cache; //!< Cache for the clock format, 0 = 24h (empty), 1 = AM, 2 = PM
        const uint8_t _clock_digit_pos[4][2] = { 
            {3, 0}, {39, 0}, {3, 55}, {39, 55} }; //!< Pixel positions of the clock digits
        const uint8_t _clock_indicator_pos[2][2] = { 
            {6, 115}, {44, 115} }; //!< Pixel positions of the clock AM/PM indicator
};

/**
 * @brief Provides a PWM interface with power saving option
 * 
 */
class PwmOutLP: public PwmOut
{

    public:

        /**
         * @brief Construct a new Pwm Out L P object
         * 
         * @param pin Which pin to use
         */
        PwmOutLP(PinName pin) : PwmOut(pin) { }

        /**
         * @brief Powers the interface up and down
         * 
         * @param on True to turn on, false to turn off
         */
        void setPower(bool on);

};


/**
 * @brief Provides methods to interact with the LCD display and other actors
 * 
 */
class DisplayService
{

    public:
        /**
         * @brief Construct a new Display Service object
         */
        DisplayService(SensorService& sensor_service, CurrentTimeService& current_time_service, 
            events::EventQueue& event_queue);

        /**
         * @brief Vibrates the motor
         * 
         * @param milliSeconds For how long to vibrate
         * 
         */
        void vibrate(uint16_t duration);

        /**
         * @brief Get the Vibration
         * 
         * @return bool true if vibrating
         */
        bool getVibration();

        /**
         * @brief Render the internal state to the LCD display
         * 
         */
        void render();

        /**
         * @brief Turns the display on or off to save energy
         * 
         * @param on True to turn on, false to turn off
         */
        void setPower(bool on);

        /**
         * @brief Return the power state
         * 
         * @return true 
         * @return false 
         */
        bool getPower();

        /**
         * @brief Set the BLE status pointer
         * 
         * @param ble_status The pointer
         */
        void setBLEStatusPtr(bool* ble_status);

        /**
         * @brief Set the BLE enc status pointer
         * 
         * @param ble__enc_status The pointer
         */
        void setBLEEncStatusPtr(bool* ble_enc_status);

        Screen screen; //!< The LCD screen model and controller

    protected:
        SensorService& _sensor_service; //!< Sensor service reference
        CurrentTimeService& _current_time_service; //! Current time service
        events::EventQueue& _event_queue; //!< Event queue
        bool* _ble_connected; //!< BLE status
        bool* _ble_encrypted; //!< BLE enc status
        DigitalOut _vibration; //!< Vibration output
        Thread _vibration_thread; //!< Thread for vibration duration
        Semaphore _vibration_trigger; //!< Interlock to trigger vibration
        uint16_t _vibration_duration; //!< Duration of the vibration in ms
        volatile bool _vibrating; //!< Indicates vibration state - read in IRQ
        int _clearVibrationToken; //!< Eventqueue token for indicator clearing timeout

        PwmOutLP _lcd_bl; //!< LCD backlight
        DigitalOut _lcd_pwr; //!< LCD power

        bool _is_on; //!< Power state

        /**
         * @brief Waits for the vibration interlock and then vibrates the motor
         * 
         */
        void threadVibration();

        /**
         * @brief Resets the vibration indicator
         * 
         */
        void clearVibration();

};

#endif
