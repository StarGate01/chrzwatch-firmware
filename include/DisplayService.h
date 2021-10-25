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

#include "HardwareConfiguration.h"
#include "CurrentTimeService.h"
#include "Screen.h"


#define LCD_TIMEOUT         4000 //!< LCD timeout in ms
#define VIBRATION_GRACE_IN  10   //!< Sensor shutoff before vibration
#define VIBRATION_GRACE_OUT 750  //!< Sensor restart grace after vibration


// Forward decalarations
class SensorService;

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
        DisplayService(SensorService& sensor_service, CurrentTimeService& current_time_service);

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
        void render(bool poweroff = false);

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

        /**
         * @brief Sets the BLE address to display
         * 
         * @param address The BLE address
         */
        void setBLEAddress(const char address[6]);

        Screen screen; //!< The LCD screen model and controller

    protected:
        SensorService& _sensor_service; //!< Sensor service reference
        CurrentTimeService& _current_time_service; //! Current time service
        events::EventQueue _event_queue; //!< Render event queue
        bool* _ble_connected = nullptr; //!< BLE status
        bool* _ble_encrypted = nullptr; //!< BLE enc status
        DigitalOut _vibration; //!< Vibration output
        Thread _vibration_thread; //!< Thread for vibration duration
        Thread _render_thread; //!< Thread for rendering
        Semaphore _vibration_trigger; //!< Interlock to trigger vibration
        uint16_t _vibration_duration = 0; //!< Duration of the vibration in ms
        volatile bool _vibrating = false; //!< Indicates vibration state - read in IRQ
        int _clearVibrationToken = -1; //!< Eventqueue token for indicator clearing timeout

        DigitalOut _lcd_bl; //!< LCD backlight
        DigitalOut _lcd_pwr; //!< LCD power

        volatile bool _is_on = false; //!< Power state

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

        /**
         * @brief Render the internal state to the LCD display, but is not IRQ safe
         * 
         */
        void unsafeRender(bool poweroff = false);

};

#endif
