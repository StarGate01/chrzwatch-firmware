/**
 * @file SensorService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in SensorService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "SensorService.h"

SensorService::SensorService(DisplayService& displayService):
    // _event_queue(16 * EVENTS_EVENT_SIZE),
    _displayService(displayService),
    _hr_value(100),
    _gyro_value{50, 50, 50},
    _battery_value(50) //,
    // _button1(PIN_BUTTON1),
    // _button2(PIN_BUTTON2)
{ 
    //Handle dispatching events
    // _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));
    //Attach interrupts
    // _button1.rise(_event_queue.event(this, &SensorService::handleButton1));
    // _button2.rise(_event_queue.event(this, &SensorService::handleButton2));
}

void SensorService::update()
{
    // _hr_value++;
    // if (_hr_value == 175) 
    // {
    //     _hr_value = 100;
    // }

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

void SensorService::handleButton1()
{
    _hr_value = 100;
    // _displayService.vibrate(200);
}

void SensorService::handleButton2()
{
    _hr_value = 50;
}