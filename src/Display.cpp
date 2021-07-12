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
    _vibration(PIN_VIBRATION),
    _vibration_trigger(1),
    _lcd_bl(PIN_LCD_BL),
    _lcd_pwr(PIN_LCD_PWR),
    _vibration_thread(osPriorityNormal, THREAD_SIZE)
{
    _vibration_thread.start(callback(this, &DisplayService::threadVibration));
    setPower(true);
    render();
}

void DisplayService::setBLEStatusPtr(bool* bleStatus)
{
    _ble_connected = bleStatus;
}

void DisplayService::setBLEEncStatusPtr(bool* bleEncStatus)
{
    _ble_encrypted = bleEncStatus;
}

void DisplayService::setBLEAddress(const char address[6])
{
    snprintf(screen._bleAddress, 19, "%02X:%02X:%02X\n %02X:%02X:%02X",
        (int)address[5], (int)address[4], (int)address[3], 
        (int)address[2], (int)address[1], (int)address[0]);
}

void DisplayService::setPower(bool on)
{
    _lcd_bl.setPower(on);
    if(on)
    {
        // Enable LCD power
        _lcd_pwr.write(1);
        _lcd_bl.write(1.0f);
    }
    else
    {
        // Prepare cache for next turn on
        _lcd_bl.write(0.f);
        // Disable LCD power
        _lcd_pwr.write(0);
    }
    _is_on = on;
}

bool DisplayService::getPower()
{
    return _is_on;
}

void DisplayService::vibrate(uint16_t duration)
{
    _vibration_duration = duration;
    _vibration_trigger.release(); // Returns even if token is already released
}

bool DisplayService::getVibration()
{
    return _vibrating;
}

void DisplayService::render()
{
    if(_is_on)
    {
        // Defer if called from IRQ context
        _event_queue.call(callback(this, &DisplayService::unsafeRender));
    }
}

void DisplayService::unsafeRender()
{
    // Update screen variables
    if(_ble_connected != nullptr) screen._bleStatus = *_ble_connected;
    if(_ble_encrypted != nullptr) screen._bleEncStatus = *_ble_encrypted;
    _current_time_service.readEpoch(screen._epochTime);
    screen._batteryPercent = _sensor_service.getBatteryPercent();
    screen._batteryRaw = _sensor_service.getBatteryRaw();
    screen._batteryCharging = _sensor_service.getBatteryCharging();
    screen._heartrate = _sensor_service.getHRValue();
    screen._stepsCadence = _sensor_service.rsc_measurement.instantaneous_cadence;
    screen._stepsTotal = _sensor_service.rsc_measurement.total_steps;

    // Render screen
    screen.render();
}

void DisplayService::threadVibration()
{
    while(true)
    {
        // Use mutex to collapse vibration requests
        _vibration_trigger.acquire();
        _vibrating = true;
        // 10 ms grace buffer for other sensor
        ThisThread::sleep_for(VIBRATION_GRACE_IN);
#       if defined(PIN_VIBRATION_INVERT)
            _vibration = 0;
#       else
            _vibration = 1;
#       endif
        ThisThread::sleep_for(_vibration_duration);
#       if defined(PIN_VIBRATION_INVERT)
            _vibration = 1;
#       else
            _vibration = 0;
#       endif
        // Wait for vibration to dampen, ensure 750 ms grace
        if(_clearVibrationToken > 0) _event_queue.cancel(_clearVibrationToken);
        _event_queue.call_in(VIBRATION_GRACE_OUT, this, &DisplayService::clearVibration);
        
    }
}

void DisplayService::clearVibration()
{
    _vibrating = false;
    _clearVibrationToken = 0;
}