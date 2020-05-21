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