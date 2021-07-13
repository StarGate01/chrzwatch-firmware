/**
 * @file CurrentTimeService.h
 * @author Christoph Honal
 * @author Takehisa Oneta
 * @brief Defines the CurrentTime BLE service and helpers
 * @version 0.1
 * @date 2020-05-21
 * 
 * Thanks to https://os.mbed.com/users/ohneta/code/BLE_CurrentTimeService/
 */

#ifndef CURRENT_TIME_SERVICE_H
#define CURRENT_TIME_SERVICE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include <events/mbed_events.h>
#include <time.h>
#include <ble/BLE.h>

#define BLE_CURRENT_TIME_CHAR_VALUE_SIZE 10

#ifndef LOW_POWER
    #define LOW_POWER 1
#endif


/**
 * @brief Provides symbols for the weekdays
 * 
 */
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
} BLE_DateTime;

/**
 * @brief Provides a BLE service to track and set the current time
 * 
 */
class CurrentTimeService 
{
    
    public:
        /**
         * @brief Construct a new Current Time Service object
         * 
         * @param ble BLE instance
         * @param event_queue Event queue for dispatching calls from interrupt
         */
        CurrentTimeService(BLE& ble, events::EventQueue &event_queue, int seconds_resolution = 1);

        /**
         * @brief Attach a attach Seconds Notify Handler
         * 
         * @param second_notify The Notify callback
         */
        void setMonotonicCallback(const Callback<void(const time_t epoch)>& second_notify);

        /**
         * @brief Sets the internal time using a date-time-stamp
         * 
         * @param dateTime The date-time-stamp
         */
        void writeDateTime(const BLE_DateTime& dateTime);

        /**
         * @brief Sets the internal time using a UNIX timestamp
         * 
         * @param epochTime The UNIX timestamp
         */
        void writeEpoch(const time_t& epochTime);

        /**
         * @brief Return the internal time as a date-time-stamp
         * 
         * @param dateTime The date-time-stamp
         */
        void readDateTime(BLE_DateTime& dateTime);

        /**
         * @brief Return the internal time as a UNIX time stamp
         * 
         * @param epochTime The UNIX timestamp
         */
        void readEpoch(time_t& epochTime);
    
    protected:
        BLE& _ble; //!< Reference to the BLE instance
        events::EventQueue &_event_queue; //!< Reference to the event queue for dispatching
        Callback<void(const time_t epoch)> _second_notify = nullptr; //!< Notify callback handler
        uint8_t _valueBytes[BLE_CURRENT_TIME_CHAR_VALUE_SIZE]; //!< Buffer for the internal time
        GattCharacteristic _currentTimeCharacteristic; //!< BLE characteristic definition
        int _ticker_event = -1;
        int _ticker_resolution = 1;

        /**
         * @brief Writes the contents of the internal buffer to the BLE GATT server
         * 
         */
        void writeBuffer();

        /**
         * @brief Handles the ticking event of the monotonic timer
         * 
         */
        void onTickerCallback(void);

        /**
         * @brief Handles incoming data update requests from the GATT client
         * 
         * @param params The new timestamp to store
         */
        virtual void onDataWritten(const GattWriteCallbackParams* params);

};
 
#endif
 