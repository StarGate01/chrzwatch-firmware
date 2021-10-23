/**
 * @file CustomSettings.h
 * @author Christoph Honal
 * @brief Defines the custom settings BLE service and helpers
 * @version 0.1
 * @date 2021-07-06
 */

#ifndef CUSTOM_SETTINGS_SERVICE_H
#define CUSTOM_SETTINGS_SERVICE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include <events/mbed_events.h>
#include <ble/BLE.h>

#define UUID_CUSTOM_SETTINGS_SERVICE 0x1901 // Spec goes up to 0x184D
#define UUID_CUSTOM_SETTINGS_CHAR    0x2C01 // Spec goes up to 0x2BC3


/**
 * @brief Provides a BLE service to read and write custom settings
 * 
 * @param T Type of the settings struct
 * 
 */
template <typename T>
class CustomSettingsService : private GattServer::EventHandler
{
    
    public:
        /**
         * @brief Construct a new Running Speed And Cadence Service object
         * 
         * @param ble BLE instance
         */
        CustomSettingsService(BLE& ble,
            const UUID& gatt_service_uuid = UUID(UUID_CUSTOM_SETTINGS_SERVICE), 
            const UUID& gatt_characteristic_uuid = UUID(UUID_CUSTOM_SETTINGS_CHAR));

        /**
         * @brief Set the update callback 
         * 
         * @param update_callback Update event callback 
         */
        void setCallback(const Callback<void(const T&)>& update_callback);

        /**
         * @brief Updates the settings
         * 
         * @param settings The data to be written
         */
        void updateSettings(const T& settings);

        /**
         * @brief Read the internal settings state
         * 
         * @param settings Target structure
         */
        void readSettings(T& settings);

    protected:
        BLE& _ble; //!< Reference to the BLE instance
        Callback<void(const T&)> _update_callback = nullptr; //!< Callback for the alert
        T _settings_value; //!< Internal settings state
        GattCharacteristic _settingsCharacteristic; //!< settings characteristic definition

        /**
         * @brief Handles incoming data update requests from the GATT client
         * 
         * @param params The new settings to store
         */
        void onDataWritten(const GattWriteCallbackParams& params) override;

};

// Define method bodies in header due to generic parameters
#include "CustomSettings_impl.h"

#endif
 