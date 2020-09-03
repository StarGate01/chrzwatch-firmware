/**
 * @file DisplayService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "DisplayService.h"
#include "SensorService.h"


DisplayService::DisplayService(SensorService &sensor_service, CurrentTimeService& current_time_service,
        events::EventQueue &event_queue):
    _sensor_service(sensor_service),
    _current_time_service(current_time_service),
    _event_queue(event_queue),
    _ble_connected(nullptr),
    _vibration(PIN_VIBRATION),
    _is_vibrating(false),
    _lcd_bl(PIN_LCD_BL),
    _lcd_pwr(PIN_LCD_PWR),
    _event_id(0)
{
    setPower(true);
}

void DisplayService::setBLEStatusPtr(bool* bleStatus)
{
    _ble_connected = bleStatus;
}

void DisplayService::setPower(bool on)
{
    _lcd_bl.setPower(on);
    if(on)
    {
        // Enable LCD power
        _lcd_pwr.write(1);
        _lcd_bl.write(1.0f);
        // Start render thread
        if(_event_id == 0)
        {
            _event_id = _event_queue.call_every(1000, this, &DisplayService::render);
        }
    }
    else
    {
        // Disable LCD power
        _lcd_pwr.write(0);
        // Stop render thread
        if(_event_id != 0) 
        {
            _event_queue.cancel(_event_id);
            _event_id = 0;
        }
    }
    _is_on = on;
}

bool DisplayService::getPower()
{
    return _is_on;
}

void DisplayService::vibrate(uint16_t duration)
{
    if(!_is_vibrating)
    {
        _is_vibrating = true;
        _vibration.write(1);
        _event_queue.call_in(duration, callback(this, &DisplayService::_endVibrate));
    }
}

void DisplayService::render()
{
    // This guard only makes sense when method is invoked manually
    if(_is_on)
    {
        if(_ble_connected != nullptr) screen.bleStatus = *_ble_connected;
        _current_time_service.readEpoch(screen.epochTime);
        screen.batteryPercent = _sensor_service.getBatteryPercent();
        screen.batteryRaw = _sensor_service.getBatteryRaw();
        screen.batteryCharging = _sensor_service.getBatteryCharging();
        screen.heartrate = _sensor_service.getHRValue();
        _sensor_service.getAccValue(screen.accData);
        screen.render();
    }
}

void DisplayService::_endVibrate()
{
    _vibration.write(0);
    _is_vibrating = false;
}