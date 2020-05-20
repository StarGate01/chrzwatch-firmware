#ifndef DEBUG_H
#define DEBUG_H

#include <mbed.h>
#include "ble/BLE.h"


inline void printError(ble_error_t error, const char* msg)
{
    printf("%s: ", msg);
    switch(error) 
    {
        case BLE_ERROR_NONE:
            printf("BLE_ERROR_NONE");
            break;
        case BLE_ERROR_BUFFER_OVERFLOW:
            printf("BLE_ERROR_BUFFER_OVERFLOW");
            break;
        case BLE_ERROR_NOT_IMPLEMENTED:
            printf("BLE_ERROR_NOT_IMPLEMENTED");
            break;
        case BLE_ERROR_PARAM_OUT_OF_RANGE:
            printf("BLE_ERROR_PARAM_OUT_OF_RANGE");
            break;
        case BLE_ERROR_INVALID_PARAM:
            printf("BLE_ERROR_INVALID_PARAM");
            break;
        case BLE_STACK_BUSY:
            printf("BLE_STACK_BUSY");
            break;
        case BLE_ERROR_INVALID_STATE:
            printf("BLE_ERROR_INVALID_STATE");
            break;
        case BLE_ERROR_NO_MEM:
            printf("BLE_ERROR_NO_MEM");
            break;
        case BLE_ERROR_OPERATION_NOT_PERMITTED:
            printf("BLE_ERROR_OPERATION_NOT_PERMITTED");
            break;
        case BLE_ERROR_INITIALIZATION_INCOMPLETE:
            printf("BLE_ERROR_INITIALIZATION_INCOMPLETE");
            break;
        case BLE_ERROR_ALREADY_INITIALIZED:
            printf("BLE_ERROR_ALREADY_INITIALIZED");
            break;
        case BLE_ERROR_UNSPECIFIED:
            printf("BLE_ERROR_UNSPECIFIED");
            break;
        case BLE_ERROR_INTERNAL_STACK_FAILURE:
            printf("BLE_ERROR_INTERNAL_STACK_FAILURE");
            break;
        case BLE_ERROR_NOT_FOUND:
            printf("BLE_ERROR_NOT_FOUND");
            break;
    }
    printf("\n");
}

inline void printAddress(const uint8_t &addr)
{
    printf("%x\n", addr);
}

inline void printMacAddress()
{
    BLEProtocol::AddressType_t addr_type;
    uint8_t address;
    BLE::Instance().gap().getAddress(&addr_type, &address);
    printf("DEVICE MAC ADDRESS: ");
    printAddress(address);
}

inline const char* phyToString(ble::phy_t phy) 
{
    switch(phy.value()) 
    {
        case ble::phy_t::LE_1M:
            return "LE 1M";
        case ble::phy_t::LE_2M:
            return "LE 2M";
        case ble::phy_t::LE_CODED:
            return "LE coded";
        default:
            return "invalid PHY";
    }
}

#endif