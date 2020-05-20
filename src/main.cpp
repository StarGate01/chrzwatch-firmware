#include <mbed.h>
#include <events/mbed_events.h>
#include "ble/BLE.h"

#include "core.h"


static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE);


void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main()
{
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);

    CoreService core(ble, event_queue);
    core.start();

    return 0;
}
