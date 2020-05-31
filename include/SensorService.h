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

/**
 * @brief Provides methods to access the various sensors
 * 
 */
class SensorService
{

    public:
        /**
         * @brief Construct a new Sensor Service object
         */
        SensorService(DisplayService& displayService);

        /**
         * @brief Update all sensor values
         * 
         */
        void update();

        /**
         * @brief Get the heartrate (BPM) value
         * 
         * @return uint8_t The heartrate
         */
        uint8_t getHRValue();

        /**
         * @brief Get the Battery value
         * 
         * @return uint8_t The battery value
         */
        uint8_t getBatteryValue();

    protected:
        // events::EventQueue _event_queue; //!< Reference to the event queue for dispatching
        // Thread _event_thread;
        DisplayService& _displayService;

        uint8_t _hr_value; //!< The internal heartrate state
        uint8_t _gyro_value[3]; //!< The internal accelerometer state
        uint8_t _battery_value; //!< The internal battery value state

        // InterruptIn _button1, _button2;

        void handleButton1();
        void handleButton2();

};

#endif
