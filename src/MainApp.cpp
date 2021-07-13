/**
 * @file MainApp.cpp
 * @author Christoph Honal
 * @brief Provides the main entrypoint and event queue
 * @version 0.1
 * @date 2020-05-21
 */

#ifdef MAIN_APP

    #include <mbed.h>
    #include <events/mbed_events.h>
    #include <ble/BLE.h>
    #include <pinmap_ex.h>

    #include "CoreService.h"
    #include "HardwareConfiguration.h"
    #include "UserSettings.h"

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

#endif


