/**
 * @file CurrentTimeService.cpp
 * @author Christoph Honal
 * @author Takehisa Oneta
 * @brief Implements the definitions from CurrentTimeService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "CurrentTimeService.h"


CurrentTimeService::CurrentTimeService(BLE &ble, events::EventQueue &event_queue, int seconds_resolution):
    _ble(ble),
    _event_queue(event_queue),
    _ticker_resolution(seconds_resolution),
    _currentTimeCharacteristic(GattCharacteristic::UUID_CURRENT_TIME_CHAR,
        _valueBytes, BLE_CURRENT_TIME_CHAR_VALUE_SIZE, BLE_CURRENT_TIME_CHAR_VALUE_SIZE,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE)
{
    // Setup buffers
    memset(_valueBytes, 0, BLE_CURRENT_TIME_CHAR_VALUE_SIZE);

    // Setup BLE service definition
    GattCharacteristic *charsTable[] = { &_currentTimeCharacteristic };
    GattService currentTimeGATT(GattService::UUID_CURRENT_TIME_SERVICE, charsTable, 1);

    // Attach GATT server and timer events
    _ble.gattServer().addService(currentTimeGATT);
    _ble.gattServer().onDataWritten(this, &CurrentTimeService::onDataWritten);
    _event_queue.call_every(_ticker_resolution * 1000, callback(this, &CurrentTimeService::onTickerCallback));
}

void CurrentTimeService::setMonotonicCallback(const Callback<void(const time_t epoch)>& second_notify)
{
    _second_notify = second_notify;
}

void CurrentTimeService::writeDateTime(const BLE_DateTime& dateTime)
{
    // Populate buffer from date-time-stamp
    *(uint16_t*)&_valueBytes[0] = dateTime.year;
    _valueBytes[2] = dateTime.month;
    _valueBytes[3] = dateTime.day;
    _valueBytes[4] = dateTime.hours;
    _valueBytes[5] = dateTime.minutes;
    _valueBytes[6] = dateTime.seconds;

    // Not supported
    _valueBytes[7] = 0;   // day of week
    _valueBytes[8] = 0;   // Fractions256
    _valueBytes[9] = 0;   // Adjust Reason

    // Update systems
    writeBuffer();
}

void CurrentTimeService::writeEpoch(const time_t& epochTime)
{
    // Populate buffer from UNIX timestamp
    struct tm tmv;
    localtime_r(&epochTime, &tmv);
    *(uint16_t *)&_valueBytes[0] = tmv.tm_year + 1900;
    _valueBytes[2] = tmv.tm_mon + 1;
    _valueBytes[3] = tmv.tm_mday;
    _valueBytes[4] = tmv.tm_hour;
    _valueBytes[5] = tmv.tm_min;
    _valueBytes[6] = tmv.tm_sec;
    _valueBytes[7] = (tmv.tm_wday == 0)? 7:(tmv.tm_wday);
    _valueBytes[8] = 0;
    _valueBytes[9] = 0;

    // Update systems
    writeBuffer();
}

void CurrentTimeService::readDateTime(BLE_DateTime& dateTime)
{
    // Convert buffer to date-time-stamp
    dateTime.year     = *(uint16_t*)&_valueBytes[0];
    dateTime.month    = _valueBytes[2];
    dateTime.day      = _valueBytes[3];
    dateTime.hours    = _valueBytes[4];
    dateTime.minutes  = _valueBytes[5];
    dateTime.seconds  = _valueBytes[6];
}

void CurrentTimeService::readEpoch(time_t& epochTime)
{
    // Convert buffer to UNIX timestamp
    struct tm timep;
    {
        timep.tm_year  = *(uint16_t *)&_valueBytes[0] - 1900;
        timep.tm_mon   = _valueBytes[2] - 1;
        timep.tm_mday  = _valueBytes[3];
        timep.tm_hour  = _valueBytes[4];
        timep.tm_min   = _valueBytes[5];
        timep.tm_sec   = _valueBytes[6];
        timep.tm_isdst = 0;
    }
    epochTime = mktime(&timep);
}

void CurrentTimeService::writeBuffer()
{
    // Update GATT server
    _ble.gattServer().write(_currentTimeCharacteristic.getValueHandle(), _valueBytes, BLE_CURRENT_TIME_CHAR_VALUE_SIZE);
}

void CurrentTimeService::onTickerCallback(void)
{
    // Add 1 second to the internal time
    time_t tmpEpochTime;
    readEpoch(tmpEpochTime);
    tmpEpochTime += _ticker_resolution;
    writeEpoch(tmpEpochTime);

    // Defer the second handler if existing
    if(_second_notify != nullptr) _event_queue.call(_second_notify, tmpEpochTime);
}

void CurrentTimeService::onDataWritten(const GattWriteCallbackParams* params)
{
    if (params->handle == _currentTimeCharacteristic.getValueHandle()) 
    {
        // Blit the received update into the buffer
        memcpy((void*)&_valueBytes, params->data, min(params->len, (uint16_t)BLE_CURRENT_TIME_CHAR_VALUE_SIZE));
    }
}
