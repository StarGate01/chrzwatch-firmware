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
// #include "GT24L24A2Y.h"
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

static JLink_RTT rtt;

FileHandle* mbed::mbed_override_console(int fd)
{
    return &rtt;
}


// I2C and SPI instance map

const PinMapI2C PinMap_I2C[] = {
    {p1, p2, 1},
    {NC, NC, NC}
};

const PinMapSPI PinMap_SPI[] = {
    {p3, p4, p5, 2},
    {p6, p7, p8, 0},
    {NC, NC, NC, NC}
};

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

// GT24L24A2Y _flash(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_ACC_CS);

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


    // char buffer[1] = { 42 };
    // _flash.read_raw(0x0, 1, buffer);
    // printf("%u\n", buffer[0]);

    // Dispatch events in main thread
    event_queue.dispatch_forever();

    return 0;
}
