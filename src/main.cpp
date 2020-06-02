/**
 * @file main.cpp
 * @author Christoph Honal
 * @brief Provides the main entrypoint and event queue
 * @version 0.1
 * @date 2020-05-21
 */

#include <mbed.h>
#include <events/mbed_events.h>
#include "ble/BLE.h"

#include "CoreService.h"

static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE); //!< The main event queue for dispatching events

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
