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
    // Setup buffers
    memset(_valueBytes, 0, UUID_ALERT_LEVEL_CHAR_VALUE_SIZE);

    // Setup BLE service definition
    GattCharacteristic *charsTable[] = { &_immediateAlertCharacteristic };
    GattService ImmediateAlertGATT(GattService::UUID_IMMEDIATE_ALERT_SERVICE, charsTable, 1);

    // Attach GATT server and timer events
    _ble.gattServer().addService(ImmediateAlertGATT);
    _ble.gattServer().setEventHandler(this);
}

void ImmediateAlertService::setCallback(const Callback<void(int)>& alert_callback)
{
    _alert_callback = alert_callback;
}

void ImmediateAlertService::onDataWritten(const GattWriteCallbackParams& params)
{
    if (params.handle == _immediateAlertCharacteristic.getValueHandle()) 
    {
        // Callback with alert value
        memcpy((void*)&_valueBytes, params.data, min(params.len, (uint16_t)UUID_ALERT_LEVEL_CHAR_VALUE_SIZE));
        if(_alert_callback != nullptr) _alert_callback((int)(_valueBytes[0]));
    }
}
