/**
 * @file Heartrate3_AFE4404.h
 * @author Christoph Honal
 * @brief Provides high-level access to the MikroElektronika HeartRate3 C API
 * @version 0.1
 * @date 2020-08-30
 */

#ifndef HEARTRATE3_AFE4404_H
#define HEARTRATE3_AFE4404_H

#include <mbed.h>
#include <events/mbed_events.h>

#include "heartrate_3.h"
#include "UnsafeI2C.h"


#define I2C_ADDRESS 0x58


/**
 * @brief Provides methods to access the heartrate sensor and API
 * 
 */
class Heartrate3_AFE4404
{

    public:
    
        /**
         * @brief Construct a new Heartrate3_AFE4404 object
         * 
         * @param sda I2C SDA pin
         * @param scl I2C SCL pin
         * @param intr ADC ready interrupt pin
         * @param reset Reset pin
         */
        Heartrate3_AFE4404(PinName sda, PinName scl, PinName intr, PinName reset);

        /**
         * @brief Starts the sensor up
         * 
         */
        void init();

        /**
         * @brief Set the power on or of
         * 
         * @param on Power state
         */
        void setPower(bool on);
        
        /**
         * @brief Get the heartrate
         * 
         * @return uint8_t the  current heartrate
         */
        uint8_t getHeartrate();
        
    protected:

        UnsafeI2C _i2c; //!< I2C interface
        InterruptIn _int_adc; //!< ADC ready interrupt
        DigitalOut _reset; //!< Reset pin

        dynamic_modes_t _dynamic_modes; //!< Dynamic register configuration
        volatile bool _is_on; //!< Tracks the power state

        void _init(); //!< Initialises the registers

        void _handleADCInterrupt(); //!< ADC interrupt handler

};

#endif
