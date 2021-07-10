/**
 * @file UnsafeSPI.h
 * @author Christoph Honal
 * @brief Implements a SPI interface without locks, which makes it unsafe for parallel usage, but enables usage in interrupt handlers (which dont support mutexes).
 * @version 0.1.1
 * @date 2020-09-02
 */

#ifndef UNSAFE_SPI_H
#define UNSAFE_SPI_H

#include <mbed.h>


/**
 * @brief Provides a SPI interface without locking
 * 
 */
class UnsafeSPI: public SPI
{

    public:

        /**
         * @brief Construct a new Unsafe S P I object
         * 
         * @param miso MOSI pin
         * @param miso MISO pin
         * @param sclk CLock pin
         * @param ssel Slave select pin
         */
        UnsafeSPI(PinName mosi, PinName miso, PinName sclk, PinName ssel = NC) : SPI(mosi, miso, sclk, ssel) { }

        void lock() override { } //!< Disabled function

        void unlock() override { } //!< Disabled function

};

#endif