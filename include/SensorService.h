/**
 * @file SensorService.h
 * @author Christoph Honal
 * @brief Defines a class to access the various sensors
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <mbed.h>
#include <events/mbed_events.h>
#include "HardwareConfiguration.h"
#include "DisplayService.h"

#define POLL_FREQUENCY 100
#define BUTTON_VIBRATION_LENGTH 75

/**
 * @brief Provides methods to access the various sensors
 * 
 */
class SensorService
{

    public:
        /**
         * @brief Construct a new Sensor Service object
         * 
         * @param display_service Reference to the display service for button vibration
         */
        SensorService(DisplayService& display_service);

        /**
         * @brief Get the heartrate (BPM) value
         * 
         * @return uint16_t The heartrate
         */
        uint16_t getHRValue();

        /**
         * @brief Get the Battery value in percent
         * 
         * @return uint8_t The battery value
         */
        uint8_t getBatteryPercent();

        /**
         * @brief Get whether the device is being charged
         * 
         * @return true Device is connected to 5V
         * @return false Device is not connected to power source
         */
        bool getBatteryCharging();

        /**
         * @brief Get whether the device is being charged
         * 
         * @return true Device is connected to 5V
         * @return false Device is not connected to power source
         */
        bool getBatteryCharging2();

    protected:
        events::EventQueue _event_queue; //!< Eventqueue for dispatich timer for polling
        Thread _event_thread; //!< Thread for polling
        DisplayService& _display_service; //!< Reference to the display service for button vibration

        AnalogIn _battery; //!< Battery voltage input
        DigitalIn _charging; //!< Is charging input
        DigitalIn _charging2; //!< Is charging input
        uint16_t _hr_value; //!< The internal heartrate state
        float _battery_value; //!< The internal battery value state
        bool _charging_value; //!< The internal charging state
        bool _charging_value2; //!< The internal charging state

        InterruptIn _button1; //!< Button 1 input
        InterruptIn _button2; //!< Button 2 input
        int _last_button1; //!< Last state of button 1 for edge detection
        int _last_button2; //!< Last state of button 2 for edge detection
        bool _cancel_timeout; //!< Whether a button timeout is already running

        void _poll(); //!< Read all sensors

        void _handleButton(); //!< Handle press of buttons
        void _handleDisplayTimeout(); //!< Handle display timeout

};

#endif
