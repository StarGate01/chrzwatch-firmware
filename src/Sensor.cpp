/**
 * @file SensorService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in SensorService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "SensorService.h"

SensorService::SensorService(DisplayService& display_service):
    _event_queue(16 * EVENTS_EVENT_SIZE),
    _display_service(display_service),
    _battery(PIN_BATTERY),
    _charging(PIN_CHARGE),
    _charging2(PIN_CHARGE2),
    _hr_value(100),
    _battery_value(50),
    _charging_value(false),
    _charging_value2(false),
    _button1(PIN_BUTTON1),
    _button2(PIN_BUTTON2),
    _cancel_timeout(true)
{ 
    // Button interrupts
    _button1.fall(callback(this, &SensorService::_handleButton));
    _button2.fall(callback(this, &SensorService::_handleButton));

    // Handle dispatching events
    _event_queue.call_every(2000, this, &SensorService::_poll);
    _event_queue.call_every(10000, this, &SensorService::_handleDisplayTimeout);
    _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));
}

uint16_t SensorService::getHRValue()
{
    return _hr_value;
}

uint8_t SensorService::getBatteryPercent()
{
    return (uint8_t)round(((_battery_value * 1.024f) - 0.330f) / 0.0009f);
}

bool SensorService::getBatteryCharging()
{
    return _charging_value;
}

bool SensorService::getBatteryCharging2()
{
    return _charging_value2;
}

void SensorService::_poll()
{
    // Update sensors
    _battery_value = _battery.read();
    _charging_value = (_charging.read() == 0);
    _charging_value2 = (_charging2.read() == 0);
}

void SensorService::_handleButton()
{
    _event_queue.call(callback(&_display_service, &DisplayService::render));
    _display_service.vibrate(BUTTON_VIBRATION_LENGTH);
    _cancel_timeout = true;
    _display_service.setPower(true);
}


void SensorService::_handleDisplayTimeout()
{
    if(!_cancel_timeout)
    {
        _display_service.setPower(false);
    }
    _cancel_timeout = false;
}