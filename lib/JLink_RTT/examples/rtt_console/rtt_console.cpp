/**
 * @file rtt_console.cpp
 * @author Christoph Honal
 * @brief Demonstrates how to retarget the mbed console to RTT
 * @version 0.1
 * @date 2021-07-09
 */

#include <mbed.h>
#include "JLink_RTT.h"


static JLink_RTT rtt;

FileHandle* mbed::mbed_override_console(int fd)
{
    return &rtt;
}

int main()
{
    printf("Hello World!\n");
    return 0;
}
