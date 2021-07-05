/**
 * @file GT24L24A2Y_Reader.h
 * @author Christoph Honal
 * @brief Defines access functions for the font chip
 * @version 0.1
 * @date 2021-07-03
 */

#ifndef GT24L24A2Y_READER_H
#define GT24L24A2Y_READER_H

#include <mbed.h>
#include <RawSerial.h>
#include <events/mbed_events.h>


/**
 * @brief Describes the data layout of a font inside the ROM
 */
struct font_layout_t
{
    uint16_t height = 0; //!< Glyph height in pixel
    uint16_t width = 0; //!< Glyph width in pixel, 0 = lattice unequal width
    uint16_t amount = 0; //!< Amount of glyphs in font
    uint32_t offset = 0; //!< Memory offset of font
    uint16_t bsize = 0; //!< Block size of font
};

/**
 * @brief Initializes a font_layout_t object using parameters for a chosen font
 * 
 * @param NAME Name of the font
 */
#define FONT(NAME) { \
    FONT_ ## NAME ## _HEIGHT, \
    FONT_ ## NAME ## _WIDTH, \
    FONT_ ## NAME ## _AMOUNT, \
    FONT_ ## NAME ## _OFFSET, \
    FONT_ ## NAME ## _BSIZE \
} 

// Font definitions

// Font Clock 4 32
#define FONT_CLOCK4_32_HEIGHT  32
#define FONT_CLOCK4_32_WIDTH   0
#define FONT_CLOCK4_32_AMOUNT  15
#define FONT_CLOCK4_32_OFFSET  0x198dde
#define FONT_CLOCK4_32_BSIZE   90

// Font Clock 4 64
#define FONT_CLOCK4_64_HEIGHT  64
#define FONT_CLOCK4_64_WIDTH   0
#define FONT_CLOCK4_64_AMOUNT  15
#define FONT_CLOCK4_64_OFFSET  0x199e0e
#define FONT_CLOCK4_64_BSIZE   322

// UI Icons
#define FONT_UIICON_HEIGHT  32
#define FONT_UIICON_WIDTH   0
#define FONT_UIICON_AMOUNT  64
#define FONT_UIICON_OFFSET  0x1a1c5f
#define FONT_UIICON_BSIZE   128


/**
 * @brief This class privides read access to the GT24L24A2Y font chip
 * 
 */
class GT24L24A2Y_Reader
{

    public:
        /**
         * @brief Construct a new font ROM reader object
         * 
         */
        GT24L24A2Y_Reader(PinName mosi, PinName miso, PinName clk, PinName cs);

        /**
         * @brief Reads a single glyph from the font ROM
         * 
         * @param font Font descriptor
         * @param glyph_id Index of the glyph in the chosen font
         * @param buffer Target buffer, must hold width * height pixels
         * @param actual_width Lattice width of the glyph or default width
         * @return int success = 0
         */
        int read(const struct font_layout_t& font, uint16_t glyph_id, char* buffer, uint16_t* actual_width);
        
        /**
         * @brief Dumps the flash content to the serial interface
         * 
         * @param serial Serial interface to use
         * @param watchdog Callback to reset the watchdog
         * 
         * @return int success = 0
         */
        void dump(RawSerial& serial, Callback<void()> watchdog = nullptr);


    protected:
        SPI _spi; //!< SPI device interface
        DigitalOut _cs; //!< Chip select pin

        /**
         * @brief Reads bytes from the font ROM (max. 255)
         * 
         * @param offset Memory offset to start from
         * @param size Amount of bytes to read
         * @param buffer Target buffer
         * @return int success = 0
         */
        int read_raw(uint32_t offset, uint16_t size, char* buffer);

};


#endif
 