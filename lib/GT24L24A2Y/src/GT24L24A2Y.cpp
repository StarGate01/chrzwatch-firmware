/**
 * @file GT24L24A2Y.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in GT24L24A2Y.h
 * @version 0.1
 * @date 2021-07-03
 */

#include "GT24L24A2Y.h"


GT24L24A2Y::GT24L24A2Y(PinName mosi, PinName miso, PinName clk, PinName cs):
    _spi(mosi, miso, clk),
    _cs(cs)
{
    _cs.write(1);
    _spi.frequency(800000); // 8MHz
}

int GT24L24A2Y::read(const struct font_layout_t& font, uint16_t glyph_id, uint8_t* buffer, uint16_t* actual_width)
{
    // Compute glyph byte offset
    uint32_t glyph_offset = font.offset + ((uint32_t)glyph_id * (uint32_t)font.bsize);
 
    // Read glyph
    int res = read_raw(glyph_offset, font.bsize, buffer);

    // Blank first two bytes in lattice mode
    if(font.width == 0) 
    {
        *actual_width = (buffer[0] * 256) + buffer[1];
        buffer[0] = 0;
        buffer[1] = 0;
    }
    else
    {
        *actual_width = font.width;
    }

    // Result should be max of tx and rx bytes
    return res;
}

int GT24L24A2Y::read_raw(uint32_t offset, uint16_t size, uint8_t* buffer)
{
    // Check offset
    if(offset > 0x1FFFFF) return 1;

    // Build command
    uint8_t command[4] = { 
        FLASH_CMD_READ, // Read Opcode
        (offset >> 16) & 0xFF, // MSB
        (offset >> 8) & 0xFF,
        offset & 0xFF // LSB
    };

    // Send 32 bit command and read response
    _cs.write(0);
    int res = _spi.write((char*)command, 4, (char*)buffer, size);
    _cs.write(1);

    // Result should be max of tx and rx bytes
    return (res == max((uint16_t)4, size))? 0 : 2;
}

void GT24L24A2Y::dump(void (*watchdog)(), void (*progress)(uint32_t offset))
{
    // Dump whole 2MB
    uint32_t offset;
    uint8_t buffer[255];
    for(offset = FLASH_USER_OFFSET; offset < 0x1FFFFF; offset += 255)
    {
        // 255 byte chunks
        read_raw(offset, 255, buffer);
        for(uint8_t j = 0; j < 255; j++) putc(buffer[j], stdout);
        fflush(stdout);
        if(watchdog != nullptr) watchdog();
        if(offset % (255 * 10) == 0 && progress != nullptr) progress(offset);
    }
    // The rest
    uint32_t rest_size = 0x1FFFFF - offset;
    read_raw(offset, rest_size, buffer);
    for(uint8_t j = 0; j < rest_size; j++) putc(buffer[j], stdout);
    fflush(stdout);
    if(watchdog != nullptr) watchdog();
    if(progress != nullptr) progress(offset + rest_size);
}

int GT24L24A2Y::erase(uint32_t offset)
{
    uint8_t command[1] = { FLASH_CMD_WRITE_ENABLE };
    _cs.write(0);
    int res = _spi.write((char*)command, 1, nullptr, 0);
    _cs.write(1);

    uint8_t command_write[4] = { 
        FLASH_CMD_SECTOR_ERASE, // Erase Opcode
        (offset >> 16) & 0xFF, // MSB
        (offset >> 8) & 0xFF,
        offset & 0xFF // LSB
    };
    _cs.write(0);
    res = _spi.write((char*)command_write, 4, nullptr, 0);
    _cs.write(1);

    command[0] = FLASH_CMD_WRITE_DISABLE;
    _cs.write(0);
    res = _spi.write((char*)command, 1, nullptr, 0);
    _cs.write(1);

    return res;
}

int GT24L24A2Y::read_deviceid(uint8_t* buffer)
{
    uint8_t command[1] = { FLASH_CMD_READ_ID };

    _cs.write(0);
    int res = _spi.write((char*)command, 4, (char*)buffer, 16);
    _cs.write(1);

    return res == 16? 0 : res;
}

int GT24L24A2Y::write(uint32_t offset, const uint8_t* buffer, uint16_t size)
{
    uint8_t command[1] = { FLASH_CMD_WRITE_ENABLE };
    _cs.write(0);
    int res = _spi.write((char*)command, 1, nullptr, 0);
    _cs.write(1);

    uint8_t command_write[4] = { 
        FLASH_CMD_PAGE_WRITE, // Write Opcode
        (offset >> 16) & 0xFF, // MSB
        (offset >> 8) & 0xFF,
        offset & 0xFF // LSB
    };
    _cs.write(0);
    res = _spi.write((char*)command_write, 4, nullptr, 0);
    res = _spi.write((char*)buffer, size, nullptr, 0);
    _cs.write(1);

    command[0] = FLASH_CMD_WRITE_DISABLE;
    _cs.write(0);
    res = _spi.write((char*)command, 1, nullptr, 0);
    _cs.write(1);

    return res;
}