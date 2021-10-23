/**
 * @file CoreService.cpp
 * @author Christoph Honal
 * @brief Implements the core BLE logic defined in CoreService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "CoreService.h"

#define RSCFF RunningSpeedAndCadenceService::RSCFeatureFlags
CoreService::CoreService(BLE& ble, events::EventQueue& event_queue):
    _event_queue(event_queue),
    _ble(ble),
    _adv_data_builder(_adv_buffer),
    _ble_hr_service(ble, 0, HeartRateService::LOCATION_WRIST),
    _ble_bat_service(ble, 0),
    _ble_time_service(ble, event_queue, 60), // 1 minute clock
    _ble_alert_service(ble),
    _ble_rsc_service(ble, (RSCFF)(
        RSCFF::INSTANTANEOUS_STRIDE_LENGTH_MEASUREMENT_SUPPORTED | 
        RSCFF::TOTAL_DISTANCE_MEASUREMENT_SUPPORTED)),
    _ble_settings_service(ble),
    _display_service(_sensor_service, _ble_time_service),
    _sensor_service(_display_service)
{ 
    _display_service.setBLEStatusPtr(&_connected);
    _display_service.setBLEEncStatusPtr(&_encrypted);
    _ble_alert_service.setCallback(callback(this, &CoreService::onAlert));
    _ble_time_service.setMonotonicCallback(callback(this, &CoreService::onMonotonic));
    _ble_settings_service.setCallback(callback(this, &CoreService::onUpdateSettings));
    _ble_settings_service.updateSettings(user_settings);
}

CoreService::~CoreService()
{
    if (_ble.hasInitialized()) _ble.shutdown();
}

void CoreService::start() 
{
    // Init BLE and attach event handlers
    _ble.gap().setEventHandler(this);
    _ble.init(this, &CoreService::onInitComplete);
    initWatchdog();

    // Setup and start ble and watchdog queue
    _event_queue.call_every(std::chrono::milliseconds(SENSOR_FREQUENCY), this, &CoreService::doUpdateGATT);
    _event_queue.call_every(std::chrono::milliseconds(15000), CoreService::kickWatchdog); // 15 sec kick leaves 5 sec headroom

    // Sensor service measures once in constructor, so update gatt at start
    doUpdateGATT();
}

void CoreService::initWatchdog()
{
    // Start the deadlock reset watchdog
    NRF_WDT->CONFIG = (WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos) | (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos);
    NRF_WDT->CRV = 20 * 32768; // 32k tick, 20 sec timeout
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk; 
    NRF_WDT->TASKS_START = 1;
}

void CoreService::startAdvertising() 
{
    // Create advertising parameters and payload
    ble::AdvertisingParameters adv_parameters(ble::advertising_type_t::CONNECTABLE_UNDIRECTED, ble::adv_interval_t(ble::millisecond_t(1000)));
    _adv_data_builder.setFlags(ble::adv_data_flags_t::BREDR_NOT_SUPPORTED | ble::adv_data_flags_t::LE_GENERAL_DISCOVERABLE);
    _adv_data_builder.setAppearance(ble::adv_data_appearance_t::GENERIC_WATCH);
    _adv_data_builder.setName(DEVICE_NAME);
    
    // Services are added at runtime, client is notified via Service Changed indicator
    // Setup advertising parameters
    _ble.gap().setAdvertisingParameters(ble::LEGACY_ADVERTISING_HANDLE, adv_parameters);

    // Setup advertising payload and start
    _ble.gap().setAdvertisingPayload(ble::LEGACY_ADVERTISING_HANDLE, _adv_data_builder.getAdvertisingData());
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
}

void CoreService::kickWatchdog()
{
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}
