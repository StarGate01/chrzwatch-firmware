#ifndef CORE_H
#define CORE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"
#include "gatt/CurrentTimeService.h"

#include "sensor.h"
#include "display.h"


const static char DEVICE_NAME[] = "CHRZwatch One dev2";

const static char INFO_MANUFACTURERS_NAME[] = "CHRZ Engineering";
const static char INFO_MODEL_NUMBER[] = "CHRZwatch One";
const static char INFO_SERIAL_NUMBER[] = "1337-420";
const static char INFO_HARDWARE_REVISION[] = "I6HRC";
const static char INFO_FIRMWARE_REVISION[] = "1.0";
const static char INFO_SOFTWARE_REVISION[] = "1.0";

static BLE_DateTime INIT_TIME = {
    1997, 7, 12, 13, 37, 44
};

class CoreService : ble::Gap::EventHandler 
{
    public:
        CoreService(BLE &ble, events::EventQueue &event_queue);
        void start();

    protected:
        events::EventQueue &_event_queue;

        bool _connected;
        BLE &_ble;
        uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
        ble::AdvertisingDataBuilder _adv_data_builder;

        HeartRateService _ble_hr_service;
        BatteryService _ble_bat_service;
        CurrentTimeService _ble_time_service;

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
