/**
 * @file UnsafeI2C.cpp
 * @author Christoph Honal
 * @brief Implements a I2C interface with non-exclusive access
 * @version 0.1
 * @date 2020-09-02
 */


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

        void lock() override { } //!< Defunct function

        void unlock() override { } //!< Defunct function

};