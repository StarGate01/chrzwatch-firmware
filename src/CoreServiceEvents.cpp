/**
 * @file CoreServiceEvents.cpp
 * @author Christoph Honal
 * @brief Implements BLE event handler definitions from CoreService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "CoreService.h"

void CoreService::onInitComplete(BLE::InitializationCompleteCallbackContext *params) 
{
    if(params->error != BLE_ERROR_NONE) 
    {
        system_reset();
        return;
    }
    startAdvertising();
}

void CoreService::onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) 
{
    _connected = false;
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
}

void CoreService::onConnectionComplete(const ble::ConnectionCompleteEvent &event) 
{
    if(event.getStatus() == BLE_ERROR_NONE) _connected = true;
}
