#include "core.h"
#include "debug.h"

void CoreService::doUpdateSensors() 
{
    if(_connected) 
    {
        _sensor_service.update();

        _ble_bat_service.updateBatteryLevel(_sensor_service.getBatteryValue());
        _ble_hr_service.updateHeartRate(_sensor_service.getHRValue());
    }
}

void CoreService::doBlink() 
{
    _display_service.blinkLed();
}