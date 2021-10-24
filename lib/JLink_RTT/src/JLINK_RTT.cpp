/**
 * @file JLink_RTT.cpp
 * @author James Wang (woodsking2@hotmail.com)
 * @author Christoph Honal
 * @brief Implements function defined in JLink_RTT.h
 * @version 0.2
 * @date 2021-07-09
 * 
 * @copyright Copyright (c) 2020-2021
 *
 */

#include "JLink_RTT.h"
#include "SEGGER_RTT.h"


JLink_RTT::JLink_RTT(const char *name) : Stream(name)
{
    SEGGER_RTT_Init();
}

int JLink_RTT::_getc()
{
    char buf;
    return (SEGGER_RTT_Read(0, &buf, 1) > 0)? buf : EOF;
}

int JLink_RTT::_putc(int c)
{
    return (SEGGER_RTT_PutChar(0, c) == 1)? c : EOF;
}

void JLink_RTT::lock()
{
    _mutex.lock();
}

void JLink_RTT::unlock()
{
    _mutex.unlock();
}

