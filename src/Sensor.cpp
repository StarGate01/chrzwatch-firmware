/**
 * @file SensorService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in SensorService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "SensorService.h"
#include "DisplayService.h"

SensorService::SensorService(DisplayService &display_service, events::EventQueue &event_queue):
    _event_queue(event_queue),
    _display_service(display_service),
    _battery(PIN_BATTERY),
    _charging(PIN_CHARGE),
    _battery_value(0),
    _charging_value(false),
    // _button1(PIN_BUTTON1),
    // _button2(PIN_BUTTON2),
    _cancel_timeout(true), //,
    _hr(HR_SDA, HR_SCL, HR_ADCREADY, HR_RESET, HR_PWR, event_queue)
    // _acc_i2c(ACC_SDA, ACC_SCL),
    // _acc_rw(_acc_i2c),
    // _acc_kx123(_acc_rw)
{ 
    // Button interrupts
    // _button1.fall(callback(this, &SensorService::_handleButton));
    // _button2.fall(callback(this, &SensorService::_handleButton));

    // Init heartrate sensor

    // Handle dispatching events
    _event_queue.call_every(SENSOR_FREQUENCY, this, &SensorService::_poll);
    // _event_queue.call_every(LCD_TIMEOUT, this, &SensorService::_handleDisplayTimeout);
    _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));
}

uint8_t SensorService::getHRValue()
{
    return _hr_value;
}

uint8_t SensorService::getBatteryPercent()
{
    return (uint8_t)max(min(round((_battery_value - 0.3f) * 1000.0f), 100.0f), 0.0f);
}

float SensorService::getBatteryRaw()
{
    return _battery_value;
}

bool SensorService::getBatteryCharging()
{
    return _charging_value;
}

void SensorService::_poll()
{
    // Update battery
    _battery_value = _battery.read();
    _charging_value = (_charging.read() == 0);

    // _hr.setPower(true);
    // ThisThread::sleep_for(HR_DURATION);
    // _hr.setPower(false);
    _hr_value = _hr.getHeartrate();
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