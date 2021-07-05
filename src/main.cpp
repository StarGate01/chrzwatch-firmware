/**
 * @file main.cpp
 * @author Christoph Honal
 * @brief Provides the main entrypoint and event queue
 * @version 0.1
 * @date 2020-05-21
 */

#include <mbed.h>


#include <events/mbed_events.h>
#include "CoreService.h"

#if defined(FLASHDUMP)
    #include "HardwareConfiguration.h"
    #include <GT24L24A2Y_Reader.h>
#else
    #include "ble/BLE.h"
#endif



#if MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED

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

#endif


static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE); //!< The main event queue for dispatching events

#if defined(FLASHDUMP)

    static GT24L24A2Y_Reader flash(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_FONT_CS); //!< Font rom interface

    static RawSerial serial(PIN_TX, PIN_RX, 115200); //!< Serial port interface

    int main()
    {
        CoreService::initWatchdog();
        flash.dump(serial, callback(CoreService::kickWatchdog));
        return 0;
    }

#else

   
    /**
     * @brief Dispatches BLE events to non-interrupt space
     * 
     * @param context Callback context
     */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) 
    {
        event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
    }

    /**
     * @brief Main entrypoint
     * 
     * @return int Return error code
     */
    int main()
    {
        // Get BLE Instance and setup BLE events to event queue handler
        BLE &ble = BLE::Instance();
        ble.onEventsToProcess(schedule_ble_events);

        // Setup core app and start it
        CoreService core(ble, event_queue);
        core.start();

        // Dispatch events in main thread
        event_queue.dispatch_forever();

        return 0;
    }

#endif