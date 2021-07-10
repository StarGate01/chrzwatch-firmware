/**
 * @file JLink_RTT.h
 * @author James Wang (woodsking2@hotmail.com)
 * @author Christoph Honal
 * @brief 使用rtt 作为stream 输出输入
 * @version 0.2.1
 * @date 2021-07-09
 *
 * @copyright Copyright (c) 2020-2021
 *
 */

// RTT folder based on JLink_V630D/Samples/RTT/SEGGER_RTT_V630d.zip 
// Does not use SEGGER_RTT_LOCK SEGGER_RTT_UNLOCK, use PlatformMutex instead

#ifndef JLINK_RTT_H
#define JLINK_RTT_H

#include <mbed.h>
#include <Stream.h>
#include <NonCopyable.h>


/**
 * @brief Provides a stream which writes to the SEGGER RTT interface
 * 
 */
class JLink_RTT : public Stream, private NonCopyable<JLink_RTT>
{

    public:
        /**
         * @brief Construct a new JLINK RTT interface
         * 
         * @param name Name of the stream
         */
        JLink_RTT(const char *name = "JLink_RTT");

        // Deleted functions - Prohibit copying

        JLink_RTT(const JLink_RTT&) = delete;
        JLink_RTT &operator=(const JLink_RTT &) = delete;
        JLink_RTT(JLink_RTT &&) = delete;
        JLink_RTT &operator=(JLink_RTT &&) = delete;

    protected:
        PlatformMutex _mutex; //!< Mutex used to coordinate resource access
        using Stream::lock; //!< Uses mutex to lock resource
        using Stream::unlock; //!< Uses mutex to unlock resource

        /**
         * @brief Reads a character from the stream
         * 
         * @return int The character read or EOF on buffer end
         */
        int _getc() override;

        /**
         * @brief Writes a character to the stream
         * 
         * @param c The character to write
         * @return The character written or EOF on buffer overflow
         */
        int _putc(int c) override; 

        /**
         * @brief Gain exclusive access to resource
         * 
         */
        void lock() override;

        /**
         * @brief Release exclusive access to resource
         * 
         */
        void unlock() override;
       

};

#endif
