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
    if(params->error != BLE_ERROR_NONE) return;
    
    // Init security bonding module
    ble_error_t error = _ble.securityManager().init(true, false, SecurityManager::IO_CAPS_NONE, NULL, false, NULL);
    if(error != BLE_ERROR_NONE) return;
    _ble.securityManager().allowLegacyPairing(false);
    _ble.securityManager().setSecurityManagerEventHandler(this);
    _ble.securityManager().setPairingRequestAuthorisation(true);

    startAdvertising();
}

void CoreService::onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) 
{
    // _ble.securityManager().purgeAllBondingState();
    _connected = false;
    _encrypted = false;
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
}

void CoreService::onConnectionComplete(const ble::ConnectionCompleteEvent &event) 
{
    if(event.getStatus() == BLE_ERROR_NONE) _connected = true;
}

void CoreService::pairingRequest(ble::connection_handle_t connectionHandle) 
{
    _ble.securityManager().acceptPairingRequest(connectionHandle);
}

void CoreService::linkEncryptionResult(ble::connection_handle_t connectionHandle, ble::link_encryption_t result)
{
    _encrypted = (result != ble::link_encryption_t::NOT_ENCRYPTED);
}