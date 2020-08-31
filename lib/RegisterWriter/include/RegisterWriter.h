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

#ifndef i2c_common_pp_h
#define i2c_common_pp_h

#include "rohm_hal2.h"       //types, DEBUG_print*, USE_*_HARDWARE_I2C

/**
* RegisterWriter class for writing sensor registers via I2C object
*/
class RegisterWriter
{
public:
    /**
    * Use pre-instantiated I2C instance for HAL.
    *
    * @param i2c_obj pre-instantiated i2c object.
    */
    RegisterWriter(I2C &i2c_obj);

    /**
    * Create a i2c instance which is connected to specified I2C pins.
    *
    * @param sda SDA pin
    * @param sdl SCL pin
    */
    RegisterWriter(PinName sda = NC, PinName scl = NC);

    ~RegisterWriter();

    /**
    * General read @buf_len value(s) to @*buf from sensor @reg in address @sad.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param *buf uint8_t[@buf_len] for read data
    * @param buf_len amount of data to read from @reg
    */
    uint8_t read_register(uint8_t sad, uint8_t reg, uint8_t* buf, uint8_t buf_len = 1);

    /**
    * FIFO Read @buf_len value(s) to @*buf from sensor @reg in address @sad.
    * Difference is the usage of stop-bit between commands.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param *buf uint8_t[@buf_len] for read data
    * @param buf_len amount of data to read from @reg
    */
    uint8_t read_fifo_register(uint8_t sad, uint8_t reg, uint8_t* buf, uint8_t buf_len);

    /**
    * Read @buf_len value(s) in high speed to @*buf from sensor @reg in address @sad.
    * Reference to kx123 specification page 24, hs 3.4mhZ mode.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param *buf uint8_t[@buf_len] for read data
    * @param buf_len amount of data to read from @reg
    */
    uint8_t hs_read_register(uint8_t sad, uint8_t reg, uint8_t* buf, uint8_t buf_len);
    
    /**
    * Write @data_len value(s) from @*data to sensor @reg in address @sad.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param *data uint8_t[@data_len] for written data
    * @param data_len amount of data to written to @reg
    */
    bool write_register(uint8_t sad, uint8_t reg, uint8_t* data, uint8_t data_len);
    bool write_register_single(uint8_t sad, uint8_t reg, uint8_t* data, uint8_t data_len);
    bool write_register_separate(uint8_t sad, uint8_t reg, uint8_t* data, uint8_t data_len);

    /**
    * Write 1 value from @data to sensor @reg in address @sad.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param data to be written
    */
    bool write_register(uint8_t sad, uint8_t reg, uint8_t data);

    /**
    * Write @data_len value(s) in high speed from @*data to sensor @reg in address @sad.
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param *data uint8_t[@data_len] for written data
    * @param data_len amount of data to written to @reg
    */
    bool hs_write_register(uint8_t sad, uint8_t reg, uint8_t* data, uint8_t data_len);

    /**
    * Read-change-write register (@sad/@reg)
    * @param sad Slave address of sensor
    * @param reg Register of sensor
    * @param mask bits to clear before applying new @bits
    * @param bits value to write
    * @return true on error, false on ok
    */
    bool change_bits(uint8_t sad, uint8_t reg, uint8_t mask, uint8_t bits);

private:
    I2C i2c_bus;
    bool self_created_i2c;
    bool write_single;   //Single command write or two command write

    void set_hs_mode_for_one_command();

};




#endif


