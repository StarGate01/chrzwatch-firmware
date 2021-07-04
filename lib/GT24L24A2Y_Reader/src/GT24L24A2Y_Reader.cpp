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
    uint32_t glyph_offset = 0x60000000 + font.offset + ((uint32_t)glyph_id * (uint32_t)font.bsize);
    char command[4] = { 
        0x03, // Read Opcode
        (glyph_offset >> 16) & 0xFF, // MSB
        (glyph_offset >> 8) & 0xFF,
        glyph_offset & 0xFF // LSB
    };

    // Send 32 bit command and read response
    _cs.write(0);
    int res = _spi.write(command, 4, buffer, font.bsize);
    _cs.write(1);

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
    return (res == max((uint16_t)4, font.bsize))? 0 : 3;
}