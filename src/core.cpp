/**
 * @file core.cpp
 * @author Christoph Honal
 * @brief Implements the core BLE logic defined in core.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "core.h"
#include "debug.h"

CoreService::CoreService(BLE &ble, events::EventQueue &event_queue):
    _event_queue(event_queue),
    _connected(false),  
    _ble(ble),
    _adv_data_builder(_adv_buffer),
    _ble_hr_service(ble, 0, HeartRateService::LOCATION_WRIST),
    _ble_bat_service(ble, 0),
    _ble_time_service(ble, event_queue),
    _display_service(LED1),
    _watchdog(Watchdog::get_instance())
{ }

void CoreService::start() 
{
    // Init BLE and attach event handlers
    _ble.gap().setEventHandler(this);
    _ble.init(this, &CoreService::onInitComplete);

    // Start the deadlock reset watchdog
    _watchdog.start(min(15000ul, _watchdog.get_max_timeout()));

    // Setup and start thread scheduler
    _event_queue.call_every(500, this, &CoreService::doBlink);
    _event_queue.call_every(1000, this, &CoreService::doUpdateSensors);
    _event_queue.call_every(10000, this, &CoreService::kickWatchdog);
    _event_queue.dispatch_forever();
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
    _watchdog.kick();
}