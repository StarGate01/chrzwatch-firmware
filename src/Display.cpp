/**
 * @file DisplayService.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "DisplayService.h"

DisplayService::DisplayService(DigitalOut led):
    _led(led)
{

}

void DisplayService::blinkLed()
{
    _led = !_led;
}

void DisplayService::render()
{
    
}
