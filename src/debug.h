/**
 * @file debug.h
 * @author Christoph Honal
 * @author Arm Limited (Mbed team)
 * @brief Provides printing functions for debugging errors
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <mbed.h>
#include "ble/BLE.h"

/**
 * @brief Print a BLE error flag in human readable form
 * 
 * @param error The BLE error
 * @param msg An additional message
 */
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

/**
 * @brief Print a MAC address
 * 
 * @param addr The MAC Address
 */
inline void printAddress(const uint8_t &addr)
{
    printf("%x\n", addr);
}

/**
 * @brief Print the BLE MAC address in human readable form
 * 
 * @param addr The BLE MAC Address
 */
inline void printMacAddress()
{
    BLEProtocol::AddressType_t addr_type;
    uint8_t address;
    BLE::Instance().gap().getAddress(&addr_type, &address);
    printf("DEVICE MAC ADDRESS: ");
    printAddress(address);
}

/**
 * @brief Convert a PHY type to a human readable string
 * 
 * @param phy The PHY type
 * @return const char* The string representation
 */
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
