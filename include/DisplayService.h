/**
 * @file DisplayService.h
 * @author Christoph Honal
 * @brief Defines a class to interact with the LCD display and other actors
 * @version 0.1
 * @date 2020-05-21
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define MBED_CONF_RTOS_PRESENT 1

#include <mbed.h>
#include <rtos.h>
#include "HardwareConfiguration.h"

/**
 * @brief Provides methods to interact with the LCD display and other actors
 * 
 */
class DisplayService
{

    public:
        /**
         * @brief Construct a new Display Service object
         */
        DisplayService();

        /**
         * @brief Vibrate the motor
         * 
         * @param milliSeconds For how long to vibrate
         * 
         */
        void vibrate(uint8_t duration);

    protected:
        DigitalOut _vibration;
        Thread _vibrationThread;
        Semaphore _vibrationTrigger;
        uint8_t _vibrationDuration;

        /**
         * @brief Render the internal state to the LCD display
         * 
         */
        void render();

        void threadVibration();

};

#endif
