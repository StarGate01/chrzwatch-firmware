/**
 * @file display.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in display.h
 * @version 0.1
 * @date 2020-05-21
 */

#include "display.h"

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
