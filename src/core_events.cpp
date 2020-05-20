#include "core.h"
#include "debug.h"

void CoreService::onInitComplete(BLE::InitializationCompleteCallbackContext *params) 
{
    if(params->error != BLE_ERROR_NONE) 
    {
        printError(params->error, "CoreService::onInitComplete");
        return;
    }
    printMacAddress();
    startAdvertising();
}

void CoreService::onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) 
{
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    _connected = false;
}

void CoreService::onConnectionComplete(const ble::ConnectionCompleteEvent &event) 
{
    if(event.getStatus() == BLE_ERROR_NONE) 
    {
        _connected = true;
    }
}
