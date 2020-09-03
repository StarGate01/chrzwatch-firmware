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
    _button1(PIN_BUTTON1),
    _button2(PIN_BUTTON2),
    _cancel_timeout(true),
    _hr(PIN_HR_SDA, PIN_HR_SCL, PIN_HR_ADCREADY, PIN_HR_RESET),
    _hr_pwr(PIN_HR_PWR),
    _event_thread(osPriorityNormal, THREAD_SIZE),
    _acc_i2c(PIN_ACC_SDA, PIN_ACC_SCL),
    _acc_rw(_acc_i2c),
    _acc_kx123(_acc_rw, KX123_DEFAULT_SLAVE_ADDRESS, KX023_WHO_AM_I_WAI_ID),
    _acc_irq(PIN_ACC_INT),
    _acc_addr(PIN_ACC_ADDR),
    _acc_cs(PIN_ACC_CS)
{ 
    // Setup buttons
    _button1.fall(callback(this, &SensorService::_handleButton));
    _button2.fall(callback(this, &SensorService::_handleButton));

    // Setup heartrate sensor
    _hr_pwr.write(1);
    _hr.init();

    // Setup acceleration sensor
    _acc_addr.write(1);
    _acc_cs.write(1);
    _acc_kx123.set_defaults();

    // Handle dispatching events
    _event_queue.call_every(SENSOR_FREQUENCY, this, &SensorService::_poll);
    _event_queue.call_every(LCD_TIMEOUT, this, &SensorService::_handleDisplayTimeout);
    _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));

    _poll();
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

void SensorService::getAccValue(float* data)
{
    _acc_kx123.getresults_g(data);
}

void SensorService::_poll()
{
    // Update battery
    _battery_value = _battery.read();
    _charging_value = (_charging.read() == 0);

    // Begin HR measuring interval
    _hr.setPower(true);
    _event_queue.call_in(HR_DURATION, callback(this, &SensorService::_finishPoll));
    _hr_value = _hr.getHeartrate();
}

void SensorService::_finishPoll()
{
    // End HR sensor interval
    _hr_value = _hr.getHeartrate();
    _hr.setPower(false);
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