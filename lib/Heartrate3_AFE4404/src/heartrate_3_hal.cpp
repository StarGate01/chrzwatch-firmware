/**
 * @file heartrate_3_hal.cpp
 * @author Christoph Honal
 * @brief Implements the HAL function defined in heartrate_3_hal.h for Mbed using native (unsafe) I2C
 * @version 0.1.1
 * @date 2020-08-30
 */

#include "heartrate_3_hal.h"

#include <mbed.h>
#include <UnsafeI2C.h>


static UnsafeI2C* _hal_i2c = nullptr;
static uint8_t _i2c_hw_address;


void hr3_hal_init(void* mbed_i2c, uint8_t address_id)
{
    _hal_i2c = (UnsafeI2C*)mbed_i2c;
    _i2c_hw_address = (address_id << 1); //Mbed uses 8 bit addresses
}

void hr3_hal_write(uint8_t *command, uint8_t *buffer, uint16_t count)
{
    uint8_t temp[BUFF_SIZE];

    uint8_t cmd_size = 1;
    uint16_t i = 0;
    uint8_t *temp_ptr = temp;
    uint8_t *buff_ptr = buffer;
    uint8_t *cmd_ptr = command;

    /* Fill the temp buffer with data*/
    while (cmd_size--) temp[i++] = *(cmd_ptr++);
    while (count--) temp[i++] = *(buff_ptr++);

    if(_hal_i2c != nullptr)
    {
        _hal_i2c->write(_i2c_hw_address, (char*)temp_ptr, i);
    }
}

void hr3_hal_read(uint8_t *command, uint8_t *buffer, uint16_t count)
{
    uint8_t cmd_size = 1;

    if(_hal_i2c != nullptr)
    {
        _hal_i2c->write(_i2c_hw_address, (char*)command, cmd_size);
        _hal_i2c->read(_i2c_hw_address, (char*)buffer, count);
    }
}

void hr3_hal_delay(uint32_t ms)
{
    ThisThread::sleep_for(ms);
}