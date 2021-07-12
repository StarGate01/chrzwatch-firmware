/**
 * @file GT24L24A2Y.h
 * @author Christoph Honal
 * @brief Defines access functions for the font chip
 * @version 0.1
 * @date 2021-07-03
 */

#ifndef GT24L24A2Y_H
#define GT24L24A2Y_H

#include <mbed.h>


// Memory layout
#define FLASH_SIZE              0x00200000 // 2 MB total
#define FLASH_SECTOR_NUM        0x00000200 // 512 sectors total
#define FLASH_SECTOR_SIZE       0x00001000 // 4 KB per sector
#define FLASH_PAGE_NUM          0x00000010 // 16 pages per sector
#define FLASH_PAGE_SIZE         0x00000100 // 256 byte per page
#define FLASH_USER_OFFSET       0x001EFFFF // Offset of rw section
#define FLASH_USER_SIZE         0x00010000 // 64 KB user total
#define FLASH_USER_PAGE_NUM     0x00000010 // 16 user sectors total

// Serial commands
#define FLASH_CMD_READ_ID       0x83 // Read device id
#define FLASH_CMD_READ          0x03 // Read
#define FLASH_CMD_FAST_READ     0x0B // Fast read
#define FLASH_CMD_WRITE_ENABLE  0x06 // Write enable
#define FLASH_CMD_WRITE_DISABLE 0x04 // Write disable
#define FLASH_CMD_PAGE_WRITE    0x02 // Write page
// #define FLASH
#define FLASH_CMD_SECTOR_ERASE  0x20 // Sector erase
#define FLASH_CMD_SLEEP_ENABLE  0xB9 // Deep sleep enable
#define FLASH_CMD_SLEEP_DISABLE 0xAB // Deep sleep disable


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
class GT24L24A2Y
{

    public:
        /**
         * @brief Construct a new font ROM reader object
         * 
         */
        GT24L24A2Y(PinName mosi, PinName miso, PinName clk, PinName cs);

        /**
         * @brief Reads a single glyph from the font ROM
         * 
         * @param font Font descriptor
         * @param glyph_id Index of the glyph in the chosen font
         * @param buffer Target buffer, must hold width * height pixels
         * @param actual_width Lattice width of the glyph or default width
         * @return int success = 0
         */
        int read(const struct font_layout_t& font, uint16_t glyph_id, uint8_t* buffer, uint16_t* actual_width);
        
        /**
         * @brief Dumps the flash content to the serial interface
         * 
         * @param watchdog Callback to reset the watchdog
         * @param progress Callback to progress function
         * @return int success = 0
         */
        void dump(void (*watchdog)() = nullptr, void (*progress)(uint32_t offset) = nullptr);

        /**
         * @brief Erases a sctor
         * 
         * @param offset sector address
         * @return 0 = success
         */
        int erase(uint32_t offset);

        /**
         * @brief Reads the device ID
         * 
         * @param buffer device id buffer (16 bytes)
         * @return int 0 = success
         */
        int read_deviceid(uint8_t* buffer);

        /**
         * @brief Reads bytes from the font ROM (max. 255)
         * 
         * @param offset Memory offset to start from
         * @param size Amount of bytes to read
         * @param buffer Target buffer
         * @return int success = 0
         */
        int read_raw(uint32_t offset, uint16_t size, uint8_t* buffer);

        /**
         * @brief Program a page
         * 
         * @param offset Page address
         * @param buffer Page content 
         * @param size Should be 256 bytes
         * @return int success = 0
         */
        int write(uint32_t offset, const uint8_t* buffer, uint16_t size);

    protected:
        SPI _spi; //!< SPI device interface
        DigitalOut _cs; //!< Chip select pin
        
};


#endif
 