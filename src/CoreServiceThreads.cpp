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
    _sensor_service.update();
    // Update GATT server
    _ble_bat_service.updateBatteryLevel(_sensor_service.getBatteryValue());
    _ble_hr_service.updateHeartRate(_sensor_service.getHRValue());
}
