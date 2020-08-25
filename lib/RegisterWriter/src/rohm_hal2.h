/*
The MIT License (MIT)
Copyright (c) 2017 Rohm Semiconductor

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef ROHM_HAL_H
#define ROHM_HAL_H

/* This file is meant for making same code work on many platforms easier. */
#ifdef __MBED__
    /* Mbed Classic */
    #include "mbed.h"               //types
    #include "I2C.h"                //I2C

    #define USE_MBED_HARDWARE_I2C
#ifdef _DEBUG
    #define DEBUG_print(...)    printf(__VA_ARGS__)
    #define DEBUG_printf(...)   printf(__VA_ARGS__)
    #define DEBUG_println(...)  printf(__VA_ARGS__);printf("\r\n")
#else
    #define DEBUG_print(...)
    #define DEBUG_printf(...)
    #define DEBUG_println(...)
#endif
#endif

#endif /* ROHM_HAL_H */
