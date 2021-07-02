/**
 * @file UnsafeI2C.h
 * @author Christoph Honal
 * @brief Implements a I2C interface without locks, which makes it unsafe for parallel usage, but enables usage in interrupt handlers (which dont support mutexes).
 * @version 0.1.1
 * @date 2020-09-02
 */

#ifndef UNSAFE_I2C_H
#define UNSAFE_I2C_H

#include <mbed.h>

/**
 * @brief Provides a I2C interface without locking
 * 
 */
class UnsafeI2C: public I2C
{

    public:

        /**
         * @brief Construct a new Unsafe I 2 C object
         * 
         * @param sda SDA Pin
         * @param scl SCL Pin
         */
        UnsafeI2C(PinName sda, PinName scl) : I2C(sda, scl) { }

        void lock() override { } //!< Disabled function

        void unlock() override { } //!< Disabled function

};

#endif