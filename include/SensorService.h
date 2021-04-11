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

#include <RegisterWriter.h>
#include <kx123.h>

#include <Heartrate3_AFE4404.h>

#define POLL_FREQUENCY 100
#define BUTTON_VIBRATION_LENGTH 75
#define ALERT_VIBRATION_LENGTH 200


// Forward decalarations
class DisplayService;


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
        SensorService(DisplayService& display_service, events::EventQueue& event_queue);

        /**
         * @brief Get the heartrate (BPM) value
         * 
         * @return uint8_t The heartrate
         */
        uint8_t getHRValue();

        /**
         * @brief Get the Battery value in percent
         * 
         * @return uint8_t The battery value
         */
        uint8_t getBatteryPercent();

        /**
         * @brief Get the Battery value raw
         * 
         * @return uint8_t The battery value
         */
        float getBatteryRaw();

        /**
         * @brief Get whether the device is being charged
         * 
         * @return true Device is connected to 5V
         * @return false Device is not connected to power source
         */
        bool getBatteryCharging();

        /**
         * @brief Get the Acceleration value
         * 
         * @param data Buffer for the acceleration data
         */
        void getAccValue(float* data);

    protected:
        events::EventQueue& _event_queue; //!< Eventqueue for dispatch timer for polling
        Thread _event_thread; //!< Thread for polling
        DisplayService& _display_service; //!< Reference to the display service for button vibration

        AnalogIn _battery; //!< Battery voltage input
        DigitalIn _charging; //!< Is charging input
        float _battery_value; //!< The internal battery value state (volts)
        bool _charging_value; //!< The internal charging state
        uint8_t _hr_value;

        InterruptIn _button1; //!< Button 1 input
        InterruptIn _button2; //!< Button 2 input
        int _last_button1; //!< Last state of button 1 for edge detection
        int _last_button2; //!< Last state of button 2 for edge detection
        bool _cancel_timeout; //!< Whether a button timeout is already running

        I2C _acc_i2c; //!< I2C interface for the acceleration sensor
        RegisterWriter _acc_rw; //!< Register access to the acceleration sensor
        KX123 _acc_kx123; //!< Access to the acceleration sensor
        InterruptIn _acc_irq; //!< Interrupt for the acceleration sensor
        DigitalOut _acc_addr; //!< Address select pin of the acceleration sensor
        DigitalOut _acc_cs; //!< Chip select pin of the acceleration sensor

        Heartrate3_AFE4404 _hr; //!< Access to heartrate sensor
        DigitalOut _hr_pwr; //!< Power pin of the heartrate sensor

        void _poll(); //!< Read all sensors
        void _finishPoll();

        void _handleButton(); //!< Handle press of buttons
        void _handleDisplayTimeout(); //!< Handle display timeout

};

#endif
