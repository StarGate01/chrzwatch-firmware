/**
 * @file SensorService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in SensorService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "SensorService.h"
#include "DisplayService.h"
#include "UserSettings.h"


SensorService::SensorService(DisplayService &display_service):
    _display_service(display_service),
    _battery(PIN_BATTERY),
    _charging(PIN_CHARGE),
    _button1(PIN_BUTTON1),
    _button2(PIN_BUTTON2),
    _button_lock(1),
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
    // Setup button interrupts
    _button1.fall(callback(this, &SensorService::handleButtonIRQ));
    _button2.fall(callback(this, &SensorService::handleButtonIRQ));

    // Setup heartrate sensor
    _hr_pwr.write(1);
    _hr.init();

    // Setup acceleration sensor
    _acc_addr.write(1);
    _acc_cs.write(1);
    _acc_irq.fall(callback(this, &SensorService::handleAccIRQ)); // Attach interrupt handler

    // Handle dispatching events
    updateUserSettings();
    _event_queue.call_every(std::chrono::minutes(MIN_SENSOR_FREQUENCY), this, &SensorService::pollPower);
    _event_queue.call_every(std::chrono::milliseconds(LCD_TIMEOUT), this, &SensorService::handleDisplayTimeout);
    _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));

    // Poll once at start
    pollPower();
}

uint8_t SensorService::getHRValue()
{
    return _hr_value;
}

float SensorService::getBatteryPercent()
{
    // Map battery from battery min - max to 0 - 100 %
    return max(min(
            (((_battery_value - BATTERY_MIN) * 100.f) / 
                (BATTERY_MAX - BATTERY_MIN)), 
        100.f), 0.f);
}

float SensorService::getBatteryRaw()
{
    return _battery_value;
}

bool SensorService::getBatteryCharging()
{
    return _charging_value;
}

void SensorService::reevaluateStepsCadence(const time_t epoch)
{
    rsc_measurement.instantaneous_cadence = _motion_count;
    // Convert cm/min to 1/256*m/s
    rsc_measurement.instantaneous_speed = (uint16_t)round(((float)(_motion_count * user_settings.sensor.step_length) / 6000.f) * 256.f);
    // Convert cm to dm
    rsc_measurement.total_distance = (rsc_measurement.total_steps * user_settings.sensor.step_length) / 10;
    // Clear running flag and set it if needed
    #define RSCF RunningSpeedAndCadenceService::RSCMeasurementFlags
    rsc_measurement.flags = (RSCF)(
        RSCF::INSTANTANEOUS_STRIDE_LENGTH_PRESENT | 
        RSCF::TOTAL_DISTANCE_PRESENT |
        ((_motion_count >= user_settings.sensor.cadence_running_thresh)? RSCF::RUNNING_NOT_WALKING : 0));
    _motion_count = 0;

    // Reset step counter on midnight
    struct tm then, now;
    localtime_r(&_last_reeval, &then);
    localtime_r(&epoch, &now);
    if(then.tm_hour == 23 && now.tm_hour == 0)
    {
        rsc_measurement.total_steps = 0;
    }
    _last_reeval = epoch;

    // Refresh display if needed
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_STEPS ||
        _display_service.screen.getState() == Screen::ScreenState::STATE_CADENCE ||
        _display_service.screen.getState() == Screen::ScreenState::STATE_DISTANCE)
    {
        _display_service.render();
    }
}

void SensorService::updateUserSettings()
{
    setupAccelerationSensor();
    rsc_measurement.instantaneous_stride_length = user_settings.sensor.step_length * 2;

    // Refresh heartrate polling
    if(_hr_poll_token != 0) _event_queue.cancel(_hr_poll_token);
    if(user_settings.sensor.hr_enable)
    {
        _hr_poll_token = _event_queue.call_every(std::chrono::minutes(user_settings.sensor.hr_frequency), 
            this, &SensorService::pollHeartrate);
    }
    else
    {
        finishPollHeartrate();
        _hr_value = 0;
        if(_display_service.screen.getState() == Screen::ScreenState::STATE_HEART)
        {
            Screen::ScreenState current_state = _display_service.screen.getState();
            current_state = static_cast<Screen::ScreenState>(static_cast<int>(current_state) + 1);
            if(current_state == Screen::ScreenState::STATE_LOOP) current_state = Screen::ScreenState::STATE_CLOCK;
            _display_service.screen.setState(current_state);
            _display_service.render();
        }
    }
    
    // Refresh display if needed
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_SETTINGS)
    {
        _display_service.render();
    }
}

void SensorService::setupAccelerationSensor()
{   
    _acc_kx123.soft_reset();
    _acc_kx123.set_config(KX122_ODCNTL_OSA_50, KX122_CNTL1_GSEL_2G, true, false, true, true); // 50Hz data rate default output, 2g range, enable motion and tilt engine
    _acc_kx123.set_cntl3_odrs(KX122_CNTL3_OTP_12P5, 0xff, KX122_CNTL3_OWUF_50); // 12.5Hz for tilt engine, 50Hz data rate for motion engine
    _acc_kx123.set_motion_detect_config(KX123_AXIS_MASK, user_settings.sensor.motion_duration, user_settings.sensor.motion_threshold); // All axes, user tresholds
    _acc_kx123.set_tilt_detect_config(KX123_X_P, 10); // X+ (turn wrist from front to top), 10/12.5Hz = 0.8s treshold
    _acc_kx123.int1_setup(0, true, false, false, false, false); // Latch interrupt 1, active low
    _acc_kx123.set_int1_interrupt_reason(KX122_MOTION_INTERRUPT | KX122_TILT_CHANGED); // Route interrupt source
    _acc_kx123.start_measurement_mode();
}

void SensorService::pollPower()
{
    // Update battery
    _battery_value = _battery.read();
    _charging_value = (_charging.read() == 0);
}

void SensorService::pollHeartrate()
{
    // Begin HR measuring interval
    _hr.setPower(true);
    _event_queue.call_in(std::chrono::seconds(min((int)user_settings.sensor.hr_duration, user_settings.sensor.hr_frequency * 60)), 
        callback(this, &SensorService::finishPollHeartrate));
}

void SensorService::finishPollHeartrate()
{
    // End HR sensor interval
    _hr_value = _hr.getHeartrate();
    _hr.setPower(false);

    // Refresh display if needed
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_HEART)
    {
        _display_service.render();
    }
}

void SensorService::handleButtonIRQ()
{
    // Collapse button presses, might happen when both sensors are touched
    if(_button_lock.try_acquire()) 
    {
        // Ensure last button press is a fixed time ago, for debouncing
        uint64_t now = get_ms_count();
        if(now - _last_button < BUTTON_DEBOUNCE) 
        {
            _button_lock.release();
            return;
        }
        _last_button = now;
        _cancel_timeout = true;

        // Trigger vibration
        if(user_settings.button_feedback == 1) 
        {
            _display_service.vibrate(BUTTON_VIBRATION_LENGTH);
        }

        // Trigger display wakeup or state change
        if(!_display_service.getPower()) 
        {
            _display_service.screen.setState(Screen::ScreenState::STATE_CLOCK);
            _display_service.setPower(true);
            _display_service.render();
        }
        else
        {
            // Advance to next screen state
            Screen::ScreenState current_state = _display_service.screen.getState();
            current_state = static_cast<Screen::ScreenState>(static_cast<int>(current_state) + 1);
            if(current_state == Screen::ScreenState::STATE_HEART && user_settings.sensor.hr_enable == 0) 
            {
                current_state = static_cast<Screen::ScreenState>(static_cast<int>(current_state) + 1);
            }
            if(current_state == Screen::ScreenState::STATE_LOOP) current_state = Screen::ScreenState::STATE_CLOCK;
            _display_service.screen.setState(current_state);
            _display_service.render();
        }

        _button_lock.release();
    }
}

void SensorService::handleAccIRQ()
{
    // Check reason
    enum e_interrupt_reason reason;
    _acc_kx123.get_interrupt_reason(&reason);
    if(reason == e_interrupt_reason::KX122_MOTION_INTERRUPT) 
    {
        // Dont count any vibration induced events
        if(!_display_service.getVibration())
        {
            _motion_count++;
            rsc_measurement.total_steps++;

            // Refresh display if needed
            Screen::ScreenState current_state = _display_service.screen.getState();
            if(current_state == Screen::ScreenState::STATE_CADENCE ||
                current_state == Screen::ScreenState::STATE_STEPS ||
                current_state == Screen::ScreenState::STATE_DISTANCE )
            {
                _display_service.render();
            }
        }
    }
    else if(reason == e_interrupt_reason::KX122_TILT_CHANGED)
    {
        // Trigger display wakeup on wrist turn
        if(!_display_service.getPower()) 
        {
            _cancel_timeout = true;
            _display_service.screen.setState(Screen::ScreenState::STATE_CLOCK);
            _display_service.setPower(true);
            _display_service.render();
        }
    }

    // Clear the interrupt latch register
    _acc_kx123.clear_interrupt();
}

void SensorService::handleDisplayTimeout()
{
    if(!_cancel_timeout)
    {
        _display_service.screen.setState(Screen::ScreenState::STATE_CLOCK);
        _display_service.render(true);
    }
    _cancel_timeout = false;
}
