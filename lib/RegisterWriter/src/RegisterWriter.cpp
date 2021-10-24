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

#include "rohm_hal2.h" //types, USE_MBED_HARDWARE_I2C, DEBUG_print*, I2C.h, I2C_SDA, I2C_SCL

#ifdef USE_MBED_HARDWARE_I2C
#include "RegisterWriter.h" //prototypes
#define I2C_WRITE 0
#define I2C_READ 1
#define MAX_DATA_LENGTH 6

RegisterWriter::RegisterWriter(UnsafeI2C& i2c_obj) : i2c_bus(i2c_obj)
{
    self_created_i2c = false;
    write_single = true;
}

RegisterWriter::~RegisterWriter()
{
    if (self_created_i2c == true)
    {
        delete &i2c_bus;
    }
}

/* i2c common functions */
uint8_t RegisterWriter::read_register(uint8_t sad, uint8_t reg, uint8_t *buf, uint8_t buf_len)
{
    uint8_t received_bytes;
    int read_ok;

    i2c_bus.write((int)((sad << 1) | I2C_WRITE), (char *)&reg, (int)1);
    read_ok = i2c_bus.read((int)((sad << 1) | I2C_READ), (char *)buf, (int)buf_len);
    if (read_ok == 0)
    { //0 == success(ack)
        received_bytes = buf_len;
    }
    else
    { //non0 == fail (nack)
        received_bytes = 0;
    }
    return (received_bytes);
}

uint8_t RegisterWriter::read_fifo_register(uint8_t sad, uint8_t reg, uint8_t *buf, uint8_t buf_len)
{
    uint8_t received_bytes;
    int read_ok;

    i2c_bus.write((int)((sad << 1) | I2C_WRITE), (char *)&reg, (int)1, true);
    read_ok = i2c_bus.read((int)((sad << 1) | I2C_READ), (char *)buf, (int)buf_len);

    if (read_ok == 0)
    { //0 == success(ack)
        received_bytes = buf_len;
    }
    else
    { //non0 == fail (nack)
        received_bytes = 0;
    }
    return (received_bytes);
}

uint8_t RegisterWriter::hs_read_register(uint8_t sad, uint8_t reg, uint8_t *buf, uint8_t buf_len)
{
    set_hs_mode_for_one_command();
    //Next read command as usual, but in highspeed
    return read_fifo_register(sad, reg, buf, buf_len);
}

/** Write data to register. */
bool RegisterWriter::write_register(uint8_t sad, uint8_t reg, uint8_t *data, uint8_t data_len)
{
    if (write_single)
        return write_register_single(sad, reg, data, data_len);
    return write_register_separate(sad, reg, data, data_len);
}

/** Write register with single write. */
bool RegisterWriter::write_register_single(uint8_t sad, uint8_t reg, uint8_t *data, uint8_t data_len)
{
    int error;

    char cmd[MAX_DATA_LENGTH + 1]; // assume max value of data_len is 6
    if (data_len > MAX_DATA_LENGTH)
    {
        return false;
    }

    cmd[0] = reg;
    for (int i = 0; i < data_len; i++)
    {
        cmd[i + 1] = data[i];
    }
    error = i2c_bus.write((int)((sad << 1) | I2C_WRITE), cmd, data_len + 1, true);
    return error != 0;
}

/** Write register with two separate writes.
 * First write register address and then continue with data. Send stop only after second write.
 */
bool RegisterWriter::write_register_separate(uint8_t sad, uint8_t reg, uint8_t *data, uint8_t data_len)
{
    int error;

    error = i2c_bus.write((int)((sad << 1) | I2C_WRITE), (char *)&reg, (int)1, true);
    error = error || i2c_bus.write((int)((sad << 1) | I2C_WRITE), (char *)data, (int)data_len, false);

    return error != 0;
}

bool RegisterWriter::write_register(uint8_t sad, uint8_t reg, uint8_t data)
{
    char data_to_send[2];
    int error;

    data_to_send[0] = reg;
    data_to_send[1] = data;
    error = i2c_bus.write((int)((sad << 1) | I2C_WRITE), &data_to_send[0], 2);

    return error != 0;
}

/* @return error true/false */
bool RegisterWriter::change_bits(uint8_t sad, uint8_t reg, uint8_t mask, uint8_t bits)
{
    uint8_t value, read;

    read = read_register(sad, reg, &value, 1);
    if (read == 1) // check length
    {
        value = value & ~mask;
        value = value | (bits & mask);
        return write_register(sad, reg, value);
    }
    else
    {
        DEBUG_printf("Read before change_bits() failed.");
        return true;
    }
}

void RegisterWriter::set_hs_mode_for_one_command()
{
#define MCODE (1 << 3)
    char temp;
    //Fullspeed mode -> highspeed mode for one command.
    i2c_bus.write((int)(MCODE), (char *)&temp, (int)0, true); //Trick to write just mcode+make nack.
}

#endif
