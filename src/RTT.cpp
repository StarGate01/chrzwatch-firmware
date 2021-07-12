/**
 * @file RTT.cpp
 * @author Christoph Honal
 * @brief Maps the default console to the RTT interface
 * @version 0.1
 * @date 2020-05-21
 */

#include <mbed.h>

#include "JLink_RTT.h"


static JLink_RTT rtt; //!< Stream which exposes the RTT interface

/**
 * @brief Override the default console used for printf etc.
 * 
 * @param fd A file id (unused)
 * @return FileHandle* A stream to perform IO on
 */
FileHandle* mbed::mbed_override_console(int fd)
{
    return &rtt;
}
