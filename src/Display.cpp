/**
 * @file DisplayService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "DisplayService.h"

DisplayService::DisplayService():
    _vibration(PIN_VIBRATION),
    _vibrationTrigger(1),
    _vibrationDuration(200)
{
    // _vibrationThread.start(callback(this, &DisplayService::threadVibration));
}

void DisplayService::vibrate(uint8_t duration)
{
    // _vibrationDuration = duration;
    // _vibrationTrigger.release();
}

void DisplayService::render()
{
    
}

void DisplayService::threadVibration()
{
    // while(true)
    // {
    //     // _vibrationTrigger.acquire();
    //     // _vibration = 1;
    //     // ThisThread::sleep_for(_vibrationDuration);
    //     // _vibration = 0;
    // }
}