#include "core.h"
#include "debug.h"

CoreService::CoreService(BLE &ble, events::EventQueue &event_queue):
    _event_queue(event_queue),
    _connected(false),  
    _ble(ble),
    _adv_data_builder(_adv_buffer),
    _ble_hr_uuid(GattService::UUID_HEART_RATE_SERVICE),
    _ble_hr_service(ble, 0, HeartRateService::LOCATION_WRIST),
    _display_service(LED1)
{
}

void CoreService::start() 
{
    // Init BLE and attach event handlers
    _ble.gap().setEventHandler(this);
    _ble.init(this, &CoreService::onInitComplete);

    // Setup and start event scheduler
    _event_queue.call_every(500, this, &CoreService::doBlink);
    _event_queue.call_every(1000, this, &CoreService::doUpdateSensors);
    _event_queue.dispatch_forever();
}


void CoreService::startAdvertising() 
{
    // Create advertising parameters and payload
    ble::AdvertisingParameters adv_parameters(
        ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
        ble::adv_interval_t(ble::millisecond_t(1000))
    );
    _adv_data_builder.setFlags();
    _adv_data_builder.setAppearance(ble::adv_data_appearance_t::GENERIC_HEART_RATE_SENSOR);
    _adv_data_builder.setLocalServiceList(mbed::make_Span(&_ble_hr_uuid, 1));
    _adv_data_builder.setName(DEVICE_NAME);

    // Setup advertising parameters
    ble_error_t error = _ble.gap().setAdvertisingParameters(
        ble::LEGACY_ADVERTISING_HANDLE,
        adv_parameters
    );
    if (error != BLE_ERROR_NONE) 
    {
        printError(error, "setAdvertisingParameters");
        return;
    }

    // Setup advertising payload
    error = _ble.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        _adv_data_builder.getAdvertisingData()
    );
    if (error != BLE_ERROR_NONE)
    {
        printError(error, "setAdvertisingPayload");
        return;
    }

    // Start advertising
    error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    if (error != BLE_ERROR_NONE) 
    {
        printError(error, "startAdvertising");
        return;
    }
}
