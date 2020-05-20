#ifndef CORE_H
#define CORE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"

#include "sensor.h"
#include "display.h"

const static char DEVICE_NAME[] = "CHRZwatch";

class CoreService : ble::Gap::EventHandler 
{
    public:
        CoreService(BLE &ble, events::EventQueue &event_queue);
        void start();

    private:
        events::EventQueue &_event_queue;

        bool _connected;
        BLE &_ble;
        uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
        ble::AdvertisingDataBuilder _adv_data_builder;

        UUID _ble_hr_uuid;
        HeartRateService _ble_hr_service;

        SensorService _sensor_service;
        DisplayService _display_service;


        void startAdvertising();

        void doUpdateSensors();
        void doBlink();

        void onInitComplete(BLE::InitializationCompleteCallbackContext *params);
        void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&);
        virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event);
};

#endif
