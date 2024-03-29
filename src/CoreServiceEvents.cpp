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

    // Read BLE MAC
    ble::own_address_type_t addr_type;
    ble::address_t addr;
    _ble.gap().getAddress(addr_type, addr);
    _display_service.setBLEAddress((const char*)addr.data());

    // // Init security bonding module
    // ble_error_t error = _ble.securityManager().init(true, false, SecurityManager::IO_CAPS_NONE, NULL, false, NULL);
    // if(error != BLE_ERROR_NONE) 
    // {
    //     system_reset();
    //     return;
    // }
    // _ble.securityManager().allowLegacyPairing(true);
    // _ble.securityManager().setSecurityManagerEventHandler(this);
    // _ble.securityManager().setPairingRequestAuthorisation(false);

    startAdvertising();
}

void CoreService::onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) 
{
    _connected = false;
    _encrypted = false;
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
}

void CoreService::onConnectionComplete(const ble::ConnectionCompleteEvent &event) 
{
    if(event.getStatus() == BLE_ERROR_NONE) _connected = true;
}

// void CoreService::linkEncryptionResult(ble::connection_handle_t connectionHandle, ble::link_encryption_t result)
// {
//     _encrypted = (result != ble::link_encryption_t::NOT_ENCRYPTED);
// }

void CoreService::onAlert(int level)
{
    if(level > 0)
    {
        _display_service.vibrate(ALERT_VIBRATION_LENGTH * level);
    }
}

void CoreService::onMonotonic(const time_t epoch)
{
    _sensor_service.reevaluateStepsCadence(epoch);
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_CLOCK)
    {
        _display_service.render();
    }
}

void CoreService::onUpdateSettings(const struct user_settings_t& settings)
{
    user_settings = settings;
    _sensor_service.updateUserSettings();
    _display_service.vibrate(BUTTON_VIBRATION_LENGTH);
    if(_display_service.screen.getState() == Screen::ScreenState::STATE_CLOCK ||
        _display_service.screen.getState() == Screen::ScreenState::STATE_SETTINGS)
    {
        _display_service.render();
    }
}