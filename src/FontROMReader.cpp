/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2021-07-03
 */

#include "DisplayService.h"


FontROMReader::FontROMReader():
    _spi(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK),
    _cs(PIN_FONT_CS)
{
    _cs.write(1);
    _spi.format(8, 0); // 8bits, CPOL=0, CPHA=0
    _spi.frequency(1000000); // 1MHz
}

int FontROMReader::read(char* buffer, uint32_t addr, uint32_t size)
{
    // Check arguments
    if(size == 0) return 1;
    if(addr > 0xFFFFFF) return 2;

    _cs.write(0);

    // Opcode and 24 bit address
    char command[4] = {
        0x03, // Opcode
        (addr >> 16) & 0xFF, // MSB
        (addr >> 8) & 0xFF,
        addr & 0xFF, // LSB
    };
    // Send 32 bit command and read response
    int res = _spi.write(command, 4, buffer, size);

    _cs.write(1);

    // Result should be max of tx and rx bytes
    return (res == max((uint32_t)4, size))? 0 : 3;
}