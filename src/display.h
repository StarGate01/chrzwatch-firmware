#ifndef DISPLAY_H
#define DISPLAY_H

#include <mbed.h>

class DisplayService
{
    public:
        DisplayService(DigitalOut led);
        void blinkLed();

    private:
        DigitalOut _led;

};

#endif
