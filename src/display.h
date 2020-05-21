#ifndef DISPLAY_H
#define DISPLAY_H

#include <mbed.h>

class DisplayService
{
    public:
        DisplayService(DigitalOut led);
        void blinkLed();

    protected:
        DigitalOut _led;

        void render();

};

#endif
