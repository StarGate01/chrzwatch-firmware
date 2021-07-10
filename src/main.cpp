/**
 * @file main.cpp
 * @author Christoph Honal
 * @brief Provides the main entrypoint and event queue
 * @version 0.1
 * @date 2020-05-21
 */

#include <mbed.h>
#include <events/mbed_events.h>
#include <ble/BLE.h>
#include <pinmap_ex.h>

#include "JLink_RTT.h"
#include "CoreService.h"
#include "UserSettings.h"
#include "HardwareConfiguration.h"


// Crash dump retention

mbed_error_status_t err_status; //!< Stores crash info

/**
 * @brief Reboot hook handler
 * 
 * @param error_context Additional error information
 */
void mbed_error_reboot_callback(mbed_error_ctx *error_context)
{
    err_status = error_context->error_status;
    printf("\n\n(before main) mbed_error_reboot_callback invoked with the following error context:\n");
    printf("    Status      : 0x%lX\n", (uint32_t)error_context->error_status);
    printf("    Value       : 0x%lX\n", (uint32_t)error_context->error_value);
    printf("    Address     : 0x%lX\n", (uint32_t)error_context->error_address);
    printf("    Reboot count: 0x%lX\n", (uint32_t)error_context->error_reboot_count);
    printf("    CRC         : 0x%lX\n", (uint32_t)error_context->crc_error_ctx);
    mbed_reset_reboot_error_info();
    mbed_reset_reboot_count();
}


// SEGGER RTT 

static JLink_RTT rtt; //!< Stream which exposes the RTT interface

/**
 * @brief Override the default console used for printf etc.
 * 
 * @param fd A file id (unused)
 * @return FileHandle* A stream to perform IO on
 */
FileHandle* mbed::mbed_override_console(int fd)
{
    return &rtt;
}


// I2C and SPI instance map

const PinMapI2C PinMap_I2C[] = 
{
    { PIN_ACC_SDA, PIN_ACC_SCL, 0 },
    { PIN_HR_SDA, PIN_HR_SCL, 1 },
    { NC, NC, NC }
}; //!< I2C interface map

const PinMapSPI PinMap_SPI[] = 
{
    { PIN_LCD_MOSI, NC, PIN_LCD_CLK, 2 },
    { PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, 2 },
    { NC, NC, NC, NC }
}; //!< SPI interface map


// Main App

static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE); //!< The main event queue for dispatching events
static BLE& ble_handle = BLE::Instance(); //!< BLE hardware instance
static CoreService core(ble_handle, event_queue); //!< App logic
struct user_settings_t user_settings; //!< User settings singleton

/**
 * @brief Dispatches BLE events to non-interrupt space
 * 
 * @param context Callback context
 */
static void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) 
{
    // Defer from IRQ
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

/**
 * @brief Main entrypoint
 * 
 * @return int Return error code
 */
int main()
{
    // Setup BLE events to event queue handler and start core
    ble_handle.onEventsToProcess(schedule_ble_events);
    core.start();

    // Dispatch events in main thread
    event_queue.dispatch_forever();

    return 0;
}
