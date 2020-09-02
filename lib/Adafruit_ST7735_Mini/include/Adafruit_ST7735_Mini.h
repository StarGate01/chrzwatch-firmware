/**
 * @file Adafruit_ST7735_Mini.h
 * @author Christoph Honal
 * @author Andrew Lindsay
 * @author Adafruit
 * @brief Provides access to a ST7735 (Mini) display.
 * @version 0.1
 * @date 2020-06-02
 */

/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ADAFRUIT_ST7735H_MINI_
#define _ADAFRUIT_ST7735H_MINI_

#include "mbed.h"
#include "Adafruit_GFX.h"

#define boolean bool

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB 0x1
#define INITR_BLACKTAB 0x2
#define INITR_18GREENTAB INITR_GREENTAB
#define INITR_18REDTAB INITR_REDTAB
#define INITR_18BLACKTAB INITR_BLACKTAB
#define INITR_144GREENTAB 0x1
#define INITR_MINI160x80 0x04
#define INITR_HALLOWING 0x05

#define ST7735_TFTWIDTH_128 128  // for 1.44 and mini
#define ST7735_TFTWIDTH_80 80    // for mini
#define ST7735_TFTHEIGHT_128 128 // for 1.44" display
#define ST7735_TFTHEIGHT_160 160 // for 1.8" and mini display

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF

/**
 * @brief Provides an Adafruit_GFX object for the ST7735 (mini) display
 * 
 */
class Adafruit_ST7735_Mini : public Adafruit_GFX
{

public:
    /**
     * @brief Construct a new Adafruit_ST7735_Mini object
     * 
     * @param mosi SPI Master out, slave in pin
     * @param miso SPI Master in, slave out pin. This may be NC
     * @param sck SPI clock pin
     * @param CS Chip select pin
     * @param RS Command/Data select pin
     * @param RST Display reset pin
     * @param w Display width
     * @param h Display height
     */
    Adafruit_ST7735_Mini(PinName mosi, PinName miso, PinName sck, PinName CS, PinName RS, PinName RST,
                         int16_t w = ST7735_TFTWIDTH_80, int16_t h = ST7735_TFTHEIGHT_160);

    /**
     * @brief Initializes a ST7735B type display
     * 
     */
    void initB(void);

    /**
     * @brief Initializes a ST7735R type display
     * 
     * @param options Which R variant display to initialize
     * @param colshift How many pixels to shift the first column
     * @param rowshift How many pixels to shift the first row
     */
    void initR(uint8_t options = INITR_GREENTAB, int8_t colshift = 0, int8_t rowshift = 0);

    /**
     * @brief Set the test address window
     * 
     * @param x0 X start
     * @param y0 Y start
     * @param x1 X end
     * @param y1 Y end
     */
    void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

    /**
     * @brief Writes a color to the display
     * 
     * @param color The color
     */
    void pushColor(uint16_t color);

    /**
     * @brief Fills the whole screen with a color
     * 
     * @param color The color
     */
    void fillScreen(uint16_t color);

    /**
     * @brief Fills one specific pixel with a color
     * 
     * @param x X pixel coordinate
     * @param y Y pixel coordinate
     * @param color The color
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    /**
     * @brief Draws a vertical line
     * 
     * @param x X position
     * @param y Y position
     * @param h Height
     * @param color The color
     */
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

    /**
     * @brief Draws a horizontal line
     * 
     * @param x X position
     * @param y Y position
     * @param w Width
     * @param color The color
     */
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

    /**
     * @brief Fills a rectangular region with a color
     * 
     * @param x X position
     * @param y Y position
     * @param w Width
     * @param h Height
     * @param color The color
     */
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /**
     * @brief Inverts the display
     * 
     * @param i Whether to invert or not
     */
    void invertDisplay(boolean i);

    /**
     * @brief Set the rotation of the display
     * 
     * @param r The rotation (0 - 3)
     */
    void setRotation(uint8_t r);

    /**
     * @brief Constructs a two-byte color from RGB values
     * 
     * @param r Red
     * @param g Green
     * @param b Blue
     * @return uint16_t The packed color 
     */
    uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

private:
    SPI lcdPort; //!< SPI bus to the display
    DigitalOut _cs; //!< Chip select pin
    DigitalOut _rs;  //!< Command/Data switch pin
    DigitalOut _rst; //!< Display reset pin
    
    uint8_t tabcolor; //!< Type specifier
    uint8_t colstart; //!< Text column start
    uint8_t rowstart; //!< Test row start

    /**
     * @brief Writes a byte to the SPI bus
     * 
     * @param b The byte
     */
    void spiwrite(uint8_t b);

    /**
     * @brief Writes a byte as a command to the SPI bus
     * 
     * @param c The command byte
     */
    void writecommand(uint8_t c);

    /**
     * @brief Writes a byte as data to the SPI bus
     * 
     * @param d The data byte
     */
    void writedata(uint8_t d);

    /**
     * @brief Writes a list of commands to the SPI bus
     * 
     * @param addr Adress of the command list
     */
    void commandList(uint8_t *addr);

    /**
     * @brief Perform common initialization
     * 
     * @param cmdList Command list used for initialization
     */
    void commonInit(uint8_t *cmdList);
};

#endif
