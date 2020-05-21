/**
 * @file display.h
 * @author Christoph Honal
 * @brief Defines a class to interact with the LCD display and other actors
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <mbed.h>

/**
 * @brief Provides methods to interact with the LCD display and other actors
 * 
 */
class DisplayService
{

    public:
        /**
         * @brief Construct a new Display Service object
         * 
         * @param led The pin of the keepalive LED
         */
        DisplayService(DigitalOut led);

        /**
         * @brief Blink the keepalive LED
         * 
         */
        void blinkLed();

    protected:
        DigitalOut _led; //!< The stored pin of the keepalive LED

        /**
         * @brief Render the internal state to the LCD display
         * 
         */
        void render();

};

#endif
