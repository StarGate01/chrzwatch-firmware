/**
 * @file ImmediateAlertService.h
 * @author Christoph Honal
 * @brief Defines the Immediate alert BLE service and helpers
 * @version 0.1
 * @date 2021-04-11
 */

#ifndef IMMEDIATE_ALERT_SERVICE_H
#define IMMEDIATE_ALERT_SERVICE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include <events/mbed_events.h>
#include <ble/BLE.h>
#include <ChainableGattServerEventHandler.h>

#define UUID_ALERT_LEVEL_CHAR_VALUE_SIZE 1


/**
 * @brief Provides a BLE service to set an immediate alert
 * 
 */
class ImmediateAlertService : private GattServer::EventHandler
{
    
    public:
        /**
         * @brief Construct a new Current Time Service object
         * 
         * @param ble BLE instance
         * @param event_queue Event queue for dispatching calls from interrupt
         */
        ImmediateAlertService(BLE& ble, ChainableGattServerEventHandler& gatt_handler);

        /**
         * @brief Set the alert callback 
         * 
         * @param alertCallback Alert event callback 
         */
        void setCallback(const Callback<void(int)>& alert_callback);

    protected:
        BLE& _ble; //!< Reference to the BLE instance
        Callback<void(int)> _alert_callback = nullptr; //!< Callback for the alert
        uint8_t _valueBytes[UUID_ALERT_LEVEL_CHAR_VALUE_SIZE]; //!< Buffer for the alert level
        GattCharacteristic _immediateAlertCharacteristic; //!< BLE characteristic definition

        /**
         * @brief Handles incoming data update requests from the GATT client
         * 
         * @param params The new timestamp to store
         */
        void onDataWritten(const GattWriteCallbackParams& params) override;
        

};
 
#endif
 