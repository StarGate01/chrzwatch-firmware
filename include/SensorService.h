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
#include "UserSettings.h"

#include <RegisterWriter.h>
#include <kx123.h>
#include <UnsafeI2C.h>

#include <Heartrate3_AFE4404.h>
#include <RunningSpeedAndCadence.h>


#define BUTTON_VIBRATION_LENGTH 75    //!< Button vibration feedback in ms
#define BUTTON_DEBOUNCE         200   //!< Minimum time between button pressen in ms
#define ALERT_VIBRATION_LENGTH  200   //!< Alert vibration length in ms
#define SENSOR_FREQUENCY        60000 //!< Sensor measuring / reporting interval in ms
#define HR_DURATION             30000 //!< Heartrate measurement interval in ms


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
        SensorService(DisplayService& display_service);

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
         * @brief Reevaluate steps cadence
         */
        void reevaluateStepsCadence();

        /**
         * @brief Updates the sensor user settings
         * 
         * @param settings New settings
         */
        void updateUserSettings(const struct user_sensor_settings_t& settings);

        RunningSpeedAndCadenceService::RSCMeasurement_t rsc_measurement; //!< Running speed and cadence measurement

    protected:
        events::EventQueue _event_queue; //!< Eventqueue for dispatch timer for polling
        Thread _event_thread; //!< Thread for polling
        DisplayService& _display_service; //!< Reference to the display service for button vibration

        AnalogIn _battery; //!< Battery voltage input
        DigitalIn _charging; //!< Is charging input
        float _battery_value; //!< The internal battery value state (volts)
        bool _charging_value; //!< The internal charging state
        uint8_t _hr_value; //!< The internal heartrate value
        uint8_t _motion_count; //!< The internal count of motion events
        uint64_t _motion_count_age; //<! Age of the motion count buffer
        struct user_sensor_settings_t _settings; //!< User settings cache

        InterruptIn _button1; //!< Button 1 input
        InterruptIn _button2; //!< Button 2 input
        uint64_t _last_button; //!< Time since button press for debouncing
        bool _cancel_timeout; //!< Whether a button timeout is already running

        UnsafeI2C _acc_i2c; //!< I2C interface for the acceleration sensor
        RegisterWriter _acc_rw; //!< Register access to the acceleration sensor
        KX123 _acc_kx123; //!< Access to the acceleration sensor
        InterruptIn _acc_irq; //!< Interrupt for the acceleration sensor
        DigitalOut _acc_addr; //!< Address select pin of the acceleration sensor
        DigitalOut _acc_cs; //!< Chip select pin of the acceleration sensor

        Heartrate3_AFE4404 _hr; //!< Access to heartrate sensor
        DigitalOut _hr_pwr; //!< Power pin of the heartrate sensor

        void setupAccellerationSensor(); //!< Initializes the accelleration sensor
        void poll(); //!< Begin to read all sensors
        void finishPoll(); //!< Called after sensor reading is complete

        void handleButtonIRQ(); //!< Handle press of buttons interrupt request
        void handleAccIRQ(); //!< Handle accelleration sensor interrupt request
        void handleDisplayTimeout(); //!< Handle display timeout

};

#endif
