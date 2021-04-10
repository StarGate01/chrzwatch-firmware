/**
 * @file CoreService.h
 * @author Christoph Honal
 * @brief Defines the core BLE logic and glues all services together
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef CORE_H
#define CORE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1
#define BLE_FEATURE_SECURITY 1

#include <mbed.h>
#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"

#include <CurrentTimeService.h>

#include "HardwareConfiguration.h"
#include "SensorService.h"
#include "DisplayService.h"


const static char DEVICE_NAME[] = TARGET_VARIANT_NAME;

/**
 * @brief Handles BLE connections, GAP advertising and contains all sub-service objects
 * 
 */
class CoreService : ble::Gap::EventHandler, public SecurityManager::EventHandler
{

    public:
        /**
         * @brief Construct a new Core Service object
         * 
         * @param _ble BLE instance
         * @param event_queue Event queue for dispatching calls from interrupt
         */
        CoreService(BLE& ble, events::EventQueue& event_queue);

        /**
         * @brief Destroy the Core Service object
         * 
         */
        virtual ~CoreService();

        /**
         * @brief Starts the subsystem threads
         * 
         */
        void start();

    protected:
        events::EventQueue& _event_queue; //!< Reference to the event queue for dispatching

        bool _connected; //!< Connection state of the BLE system
        bool _encrypted; //!< BLE link encryption state
        BLE& _ble; //!< Reference to the BLE instance
        uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE]; //!< BLE GAP advertising buffer
        ble::AdvertisingDataBuilder _adv_data_builder; //!< BLE GAP factory

        HeartRateService _ble_hr_service; //!< BLE heartrate service
        BatteryService _ble_bat_service; //!< BLE battery service
        CurrentTimeService _ble_time_service; //!< BLE current time service

        DisplayService _display_service; //!< Display subsystem
        SensorService _sensor_service; //!< Sensor subsystem

        /**
         * @brief Starts the BLE GAP advertising
         * 
         */
        void startAdvertising();

        /**
         * @brief Update GATT thread
         * 
         */
        void doUpdateGATT();

        /**
         * @brief Handles the resulting link encryption event
         * 
         * @param connectionHandle Handle to the BLE connection
         * @param result The type of encryption
         */
        virtual void linkEncryptionResult(ble::connection_handle_t connectionHandle, ble::link_encryption_t result);

        /**
         * @brief Handles the BLE init completion event
         * 
         * @param params Callback context, contains error information
         */
        void onInitComplete(BLE::InitializationCompleteCallbackContext *params);
        
        /**
         * @brief handles the BLE disconnection completed event
         * 
         * @param event Event information
         * 
         */
        void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event);

        /**
         * @brief Handles the BLE connection completed event
         * 
         * @param event Event information
         */
        virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event);

        /**
         * @brief Restarts the deadlock watchdog
         * 
         */
        void kickWatchdog();

};

#endif
