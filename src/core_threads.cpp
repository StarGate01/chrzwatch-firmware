/**
 * @file core_threads.cpp
 * @author Christoph Honal
 * @brief Implements thread definitions from core.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "core.h"
#include "debug.h"

void CoreService::doUpdateSensors() 
{
    if(_connected) 
    {
        _sensor_service.update();

        // Update GATT server
        _ble_bat_service.updateBatteryLevel(_sensor_service.getBatteryValue());
        _ble_hr_service.updateHeartRate(_sensor_service.getHRValue());
    }
}

void CoreService::doBlink() 
{
    _display_service.blinkLed();
}
