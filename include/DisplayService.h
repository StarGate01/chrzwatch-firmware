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
// #include <GT24L24A2Y_Reader.h>

#include "HardwareConfiguration.h"
#include "CurrentTimeService.h"


#define LCD_TIMEOUT         5000 //!< LCD timeout in ms
#define VIBRATION_GRACE_IN  10   //!< Sensor shutoff before vibration
#define VIBRATION_GRACE_OUT 750  //!< Sensor restart grace after vibration

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
         * @brief Construct a new Screen Model object
         * 
         */
        Screen();

        /**
         * @brief Renders the model to a LCD
         */
        void render();

        Adafruit_ST7735_Mini lcd; //!< LCD output
        time_t epochTime; //!< Current time
        uint8_t batteryPercent; //!< Battery remaining in percent
        uint8_t heartrate; //!< Heartrate
        float batteryRaw; //!< Battery remaining raw (volts)
        bool batteryCharging; //!< Battery charging state
        bool bleStatus; //!< Bluetooth status
        bool bleEncStatus; //!< Bluetooth Enc status
        uint8_t stepsCadence; //!< Steps cadence
        uint32_t stepsTotal; //!< Steps total
        uint32_t test;
        int test2;

    protected:
        Semaphore _display_guard; //!< Serialize display bus access
        // GT24L24A2Y_Reader _font_reader; //!< Font ROM interface

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
