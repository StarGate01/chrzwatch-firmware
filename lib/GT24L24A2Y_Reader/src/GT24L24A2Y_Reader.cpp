/**
 * @file GT24L24A2Y_Reader.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in GT24L24A2Y_Reader.h
 * @version 0.1
 * @date 2021-07-03
 */

#include "GT24L24A2Y_Reader.h"


GT24L24A2Y_Reader::GT24L24A2Y_Reader(PinName mosi, PinName miso, PinName clk, PinName cs):
    _spi(mosi, miso, clk),
    _cs(cs)
{
    _cs.write(1);
    _spi.format(8, 0); // 8bits, CPOL=0, CPHA=0
    _spi.frequency(1000000); // 1MHz
}

int GT24L24A2Y_Reader::read(const struct font_layout_t& font, uint16_t glyph_id, char* buffer, uint16_t* actual_width)
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

int GT24L24A2Y_Reader::read_raw(uint32_t offset, uint16_t size, char* buffer)
{
    // Check offset
    if(offset > 0x1FFFFF) return 1;

    // Build command
    char command[4] = { 
        0x03, // Read Opcode
        (offset >> 16) & 0xFF, // MSB
        (offset >> 8) & 0xFF,
        offset & 0xFF // LSB
    };

    // Send 32 bit command and read response
    _cs.write(0);
    int res = _spi.write(command, 4, buffer, size);
    _cs.write(1);

    // Result should be max of tx and rx bytes
    return (res == max((uint16_t)4, size))? 0 : 2;
}

void GT24L24A2Y_Reader::dump(RawSerial& serial, Callback<void()> watchdog)
{
    while(true)
    {
        // Wait for "d" command
        while(serial.getc() != 0x64) 
        {
            if(watchdog != nullptr) watchdog();
            ThisThread::sleep_for(100);
        }

        // Dump whole 2MB
        uint32_t offset;
        char buffer[255];
        for(offset = 0; offset < 0x1FFFFF; offset += 255)
        {
            // 255 byte chunks
            read_raw(offset, 255, buffer);
            for(uint8_t j = 0; j < 255; j++) serial.putc(buffer[j]);
            if(watchdog != nullptr) watchdog();
        }
        // The rest
        uint32_t rest_size = 0x1FFFFF - offset;
        read_raw(offset, rest_size, buffer);
        for(uint8_t j = 0; j < rest_size; j++) serial.putc(buffer[j]);
        if(watchdog != nullptr) watchdog();
    }
}
