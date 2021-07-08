/**
 * @file SensorService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in SensorService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "SensorService.h"
#include "DisplayService.h"

SensorService::SensorService(DisplayService &display_service):
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
    setupAccellerationSensor();

    // Handle dispatching events
    _event_queue.call_every(SENSOR_FREQUENCY, this, &SensorService::poll);
    _event_queue.call_every(LCD_TIMEOUT, this, &SensorService::handleDisplayTimeout);
    _event_thread.start(callback(&_event_queue, &EventQueue::dispatch_forever));

    // Poll once at start
    poll();
}

uint8_t SensorService::getHRValue()
{
    return _hr_value;
}

uint8_t SensorService::getBatteryPercent()
{
    // Map battery from battery min - max to 0 - 100 %
    return (uint8_t)
        max(min(
            (int)round(
                ((_battery_value - BATTERY_MIN) * 100.f) / 
                (BATTERY_MAX - BATTERY_MIN)), 
        100), 0);
}

float SensorService::getBatteryRaw()
{
    return _battery_value;
}

bool SensorService::getBatteryCharging()
{
    return _charging_value;
}

void SensorService::reevaluateStepsCadence()
{
    uint64_t now = get_ms_count();
    if(now - _motion_count_age > 60000) // 1 minute
    {
        rsc_measurement.instantaneous_cadence = _motion_count;
        // Convert cm/min to 1/256*m/s
        rsc_measurement.instantaneous_speed = (uint16_t)round(((float)(_motion_count * _settings.step_length) / 6000.f) * 256.f);
        // Convert cm to dm
        rsc_measurement.total_distance = (rsc_measurement.total_steps * _settings.step_length) / 10;
        // Clear running flag and set it if needed
        #define RSCF RunningSpeedAndCadenceService::RSCMeasurementFlags
        rsc_measurement.flags = (RSCF)(
            RSCF::INSTANTANEOUS_STRIDE_LENGTH_PRESENT | 
            RSCF::TOTAL_DISTANCE_PRESENT |
            ((_motion_count >= _settings.cadence_running_thresh)? RSCF::RUNNING_NOT_WALKING : 0));
        _motion_count = 0;
        _motion_count_age = now;

        // Refresh display if needed
        if(_display_service.screen.getState() == Screen::ScreenState::STATE_STEPS ||
            _display_service.screen.getState() == Screen::ScreenState::STATE_CADENCE ||
            _display_service.screen.getState() == Screen::ScreenState::STATE_DISTANCE)
        {
            _display_service.render();
        }
    }
}

void SensorService::updateUserSettings(const struct user_sensor_settings_t& settings)
{
    _settings = settings;
    setupAccellerationSensor();
    rsc_measurement.instantaneous_stride_length = _settings.step_length * 2;

    // Refresh display if needed
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_SETTINGS)
    {
        _display_service.render();
    }
}

void SensorService::setupAccellerationSensor()
{   
    _acc_kx123.soft_reset();
    _acc_kx123.set_config(KX122_ODCNTL_OSA_50, KX122_CNTL1_GSEL_2G, true, false, true); // 50Hz data rate default output, 2g range
    _acc_kx123.set_cntl3_odrs(0xff, 0xff, KX122_CNTL3_OWUF_50); // 50Hz data rate for motion engine
    _acc_kx123.set_motion_detect_config(KX122_INC2_WUE_MASK, _settings.motion_duration, _settings.motion_threshold); // All axes
    _acc_kx123.int1_setup(0, true, false, false, false, false); // Latch interrupt 1, active low
    _acc_kx123.set_int1_interrupt_reason(KX122_MOTION_INTERRUPT); // Route interrupt source
    _acc_kx123.start_measurement_mode();
}

void SensorService::poll()
{
    // Update battery
    _battery_value = _battery.read();
    _charging_value = (_charging.read() == 0);

    // Begin HR measuring interval
    _hr.setPower(true);
    _event_queue.call_in(HR_DURATION, callback(this, &SensorService::finishPoll));

    // Refresh display if needed
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_SETTINGS)
    {
        _display_service.render();
    }
}

void SensorService::finishPoll()
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
    // Ensure last button press is a fixed time ago, for debouncing
    uint64_t now = get_ms_count();
    if(now - _last_button < BUTTON_DEBOUNCE) return;
    _last_button = now;
    _cancel_timeout = true;

    // Trigger vibration
    _display_service.vibrate(BUTTON_VIBRATION_LENGTH);

    // Trigger display wakeup or state change
    if(!_display_service.getPower()) _display_service.setPower(true);
    else
    {
        // Advance to next screen state
        Screen::ScreenState current_state = _display_service.screen.getState();
        (*(int*)&current_state)++;
        if(current_state == Screen::ScreenState::STATE_LOOP) current_state = Screen::ScreenState::STATE_CLOCK;
        _display_service.screen.setState(current_state);

        // Defer from IRQ context
        _event_queue.call(&_display_service, &DisplayService::render);
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
            if(_display_service.screen.getState() == Screen::ScreenState::STATE_HEART)
            {
                // Defer from IRQ context
                _event_queue.call(&_display_service, &DisplayService::render);
            }
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
        _display_service.setPower(false);
    }
    _cancel_timeout = false;
}
