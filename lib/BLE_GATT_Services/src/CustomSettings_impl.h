/**
 * @file CustomSettings.cpp
 * @author Christoph Honal
 * @brief Implements the definitions from CustomSettings.h
 * @version 0.1
 * @date 2021-07-06
 */

#include <functional>

#include "CustomSettings.h"


template <class T>
CustomSettingsService<T>::CustomSettingsService(BLE& ble,
        const UUID& gatt_service_uuid, const UUID& gatt_characteristic_uuid):
    _ble(ble),
    _settingsCharacteristic(gatt_characteristic_uuid,
        (uint8_t*)&_settings_value, sizeof(T), sizeof(T),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE)
{
    // Setup BLE service definition
    GattCharacteristic *charsTable[] = { &_settingsCharacteristic };
    GattService CustomSettingsGATT(gatt_service_uuid, charsTable, 1);

    // Attach GATT server
    _ble.gattServer().addService(CustomSettingsGATT);
    _ble.gattServer().onDataWritten(this, &CustomSettingsService<T>::onDataWritten);
}

template <class T>
void CustomSettingsService<T>::setCallback(const Callback<void(const T&)>& update_callback)
{
    _update_callback = update_callback;
}

template <class T>
void CustomSettingsService<T>::updateSettings(const T& settings)
{
    _settings_value = settings;
    _ble.gattServer().write(_settingsCharacteristic.getValueHandle(), (uint8_t*)&_settings_value, sizeof(T));
}

template <class T>
void CustomSettingsService<T>::readSettings(T& settings)
{
    settings = _settings_value;
}

template <class T>
void CustomSettingsService<T>::onDataWritten(const GattWriteCallbackParams* params)
{
    if (params->handle == _settingsCharacteristic.getValueHandle()) 
    {
        // Callback with update value
        memcpy(&_settings_value, params->data, min(params->len, (uint16_t)(sizeof(T))));
        if(_update_callback != nullptr) _update_callback(_settings_value);
    }
}
