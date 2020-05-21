/**
 * @file sensor.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in sensor.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "sensor.h"

SensorService::SensorService():
    _hr_value(100),
    _gyro_value{50, 50, 50},
    _battery_value(50)
{

}

void SensorService::update()
{
    _hr_value++;
    if (_hr_value == 175) 
    {
        _hr_value = 100;
    }

    _battery_value--;
    if (_battery_value == 10) 
    {
        _battery_value = 75;
    }
}

uint8_t SensorService::getHRValue()
{
    return _hr_value;
}

uint8_t SensorService::getBatteryValue()
{
    return _battery_value;
}
