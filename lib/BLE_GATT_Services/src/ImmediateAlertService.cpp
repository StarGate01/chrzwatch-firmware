/**
 * @file ImmediateAlertService.cpp
 * @author Christoph Honal
 * @brief Implements the definitions from ImmediateAlertService.h
 * @version 0.1
 * @date 2021-04-11
 */

#include "ImmediateAlertService.h"


ImmediateAlertService::ImmediateAlertService(BLE &ble):
    _ble(ble),
    _immediateAlertCharacteristic(GattCharacteristic::UUID_ALERT_LEVEL_CHAR,
        _valueBytes, UUID_ALERT_LEVEL_CHAR_VALUE_SIZE, UUID_ALERT_LEVEL_CHAR_VALUE_SIZE,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)
{
    memset(_valueBytes, 0, sizeof(uint8_t)*UUID_ALERT_LEVEL_CHAR_VALUE_SIZE);

    // Setup BLE service definition
    GattCharacteristic *charsTable[] = { &_immediateAlertCharacteristic };
    GattService ImmediateAlertService(GattService::UUID_IMMEDIATE_ALERT_SERVICE, charsTable, sizeof(charsTable)/sizeof(GattCharacteristic*));

    // Attach GATT server and timer events
    _ble.gattServer().addService(ImmediateAlertService);
    _ble.gattServer().onDataWritten(this, &ImmediateAlertService::onDataWritten);
}

void ImmediateAlertService::setCallback(Callback<void(int)> alertCallback)
{
    _alert_callback = alertCallback;
}

void ImmediateAlertService::onDataWritten(const GattWriteCallbackParams* params)
{
    if (params->handle == _immediateAlertCharacteristic.getValueHandle()) 
    {
        // Callback with alert value
        memcpy((void*)&_valueBytes, params->data, params->len);
        _alert_callback((int)(_valueBytes[0]));
    }
}
