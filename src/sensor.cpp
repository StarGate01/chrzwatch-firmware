#include "sensor.h"

SensorService::SensorService():
    _hr_value(100),
    _gyro_value({50, 50, 50}),
    _battery_value(50),
    _hr_counter(100)
{

}

void SensorService::update()
{
    _hr_counter++;
    if (_hr_counter == 175) 
    {
        _hr_counter = 100;
    }

    _hr_value = _hr_counter;
}

uint8_t SensorService::getHRValue()
{
    return _hr_value;
}

uint8_t SensorService::getBatteryValue()
{
    return _battery_value;
}