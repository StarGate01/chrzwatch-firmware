/**
 * @file CoreServiceThreads.cpp
 * @author Christoph Honal
 * @brief Implements thread definitions from CoreService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "CoreService.h"

void CoreService::doUpdateSensors() 
{
    _ble_bat_service.updateBatteryLevel(_sensor_service.getBatteryPercent());
    _ble_hr_service.updateHeartRate(_sensor_service.getHRValue());
}

void CoreService::doUpdateDisplay()
{
    if(_display_service.getPower())
    {
        _ble_time_service.readEpoch(_display_service.screen.epochTime);
        _display_service.screen.batteryValue = _sensor_service.getBatteryPercent();
        _display_service.screen.batteryCharging = _sensor_service.getBatteryCharging();
        _display_service.screen.batteryCharging2 = _sensor_service.getBatteryCharging2();
        _display_service.render();
    }
}