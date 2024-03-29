/**
 * @file Adafruit_ST7735_Mini.cpp
 * @author Christoph Honal
 * @author Andrew Lindsay
 * @author Adafruit
 * @brief Implements the functionality defined in Adafruit_ST7735_Mini.h
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

#include <mbed.h>

#include "Adafruit_ST7735_Mini.h"


inline uint16_t swapcolor(uint16_t x)
{
    return (x << 11) | (x & 0x07E0) | (x >> 11);
}

// Constructor
Adafruit_ST7735_Mini::Adafruit_ST7735_Mini(PinName mosi, PinName miso, PinName sck, PinName cs, PinName rs, PinName rst,
                                           int frequency, int16_t w, int16_t h)
    : lcdPort(mosi, miso, sck), _cs(cs), _rs(rs), _rst(rst), Adafruit_GFX(w, h)
{
    lcdPort.frequency(frequency);
}

void Adafruit_ST7735_Mini::writecommand(uint8_t c)
{
    _rs = 0;
    _cs = 0;
    lcdPort.write(c);
    _cs = 1;
}

void Adafruit_ST7735_Mini::writedata(uint8_t c)
{
    _rs = 1;
    _cs = 0;
    lcdPort.write(c);

    _cs = 1;
}

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static unsigned char
    Bcmd[] = {                     // Initialization commands for 7735B screens
        18,                        // 18 commands in list:
        ST7735_SWRESET, DELAY,     //  1: Software reset, no args, w/delay
        50,                        //     50 ms delay
        ST7735_SLPOUT, DELAY,      //  2: Out of sleep mode, no args, w/delay
        255,                       //     255 = 500 ms delay
        ST7735_COLMOD, 1 + DELAY,  //  3: Set color mode, 1 arg + delay:
        0x05,                      //     16-bit color
        10,                        //     10 ms delay
        ST7735_FRMCTR1, 3 + DELAY, //  4: Frame rate control, 3 args + delay:
        0x00,                      //     fastest refresh
        0x06,                      //     6 lines front porch
        0x03,                      //     3 lines back porch
        10,                        //     10 ms delay
        ST7735_MADCTL, 1,          //  5: Memory access ctrl (directions), 1 arg:
        0x08,                      //     Row addr/col addr, bottom to top refresh
        ST7735_DISSET5, 2,         //  6: Display settings #5, 2 args, no delay:
        0x15,                      //     1 clk cycle nonoverlap, 2 cycle gate
                                   //     rise, 3 cycle osc equalize
        0x02,                      //     Fix on VTL
        ST7735_INVCTR, 1,          //  7: Display inversion control, 1 arg:
        0x0,                       //     Line inversion
        ST7735_PWCTR1, 2 + DELAY,  //  8: Power control, 2 args + delay:
        0x02,                      //     GVDD = 4.7V
        0x70,                      //     1.0uA
        10,                        //     10 ms delay
        ST7735_PWCTR2, 1,          //  9: Power control, 1 arg, no delay:
        0x05,                      //     VGH = 14.7V, VGL = -7.35V
        ST7735_PWCTR3, 2,          // 10: Power control, 2 args, no delay:
        0x01,                      //     Opamp current small
        0x02,                      //     Boost frequency
        ST7735_VMCTR1, 2 + DELAY,  // 11: Power control, 2 args + delay:
        0x3C,                      //     VCOMH = 4V
        0x38,                      //     VCOML = -1.1V
        10,                        //     10 ms delay
        ST7735_PWCTR6, 2,          // 12: Power control, 2 args, no delay:
        0x11, 0x15,
        ST7735_GMCTRP1, 16,     // 13: Magical unicorn dust, 16 args, no delay:
        0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
        0x21, 0x1B, 0x13, 0x19, //      these config values represent)
        0x17, 0x15, 0x1E, 0x2B,
        0x04, 0x05, 0x02, 0x0E,
        ST7735_GMCTRN1, 16 + DELAY, // 14: Sparkles and rainbows, 16 args + delay:
        0x0B, 0x14, 0x08, 0x1E,     //     (ditto)
        0x22, 0x1D, 0x18, 0x1E,
        0x1B, 0x1A, 0x24, 0x2B,
        0x06, 0x06, 0x02, 0x0F,
        10,                   //     10 ms delay
        ST7735_CASET, 4,      // 15: Column addr set, 4 args, no delay:
        0x00, 0x02,           //     XSTART = 2
        0x00, 0x81,           //     XEND = 129
        ST7735_RASET, 4,      // 16: Row addr set, 4 args, no delay:
        0x00, 0x02,           //     XSTART = 1
        0x00, 0x81,           //     XEND = 160
        ST7735_NORON, DELAY,  // 17: Normal display on, no args, w/delay
        10,                   //     10 ms delay
        ST7735_DISPON, DELAY, // 18: Main screen turn on, no args, w/delay
        255},                 //     255 = 500 ms delay

    Rcmd1[] = {                // Init for 7735R, part 1 (red or green tab)
        15,                    // 15 commands in list:
        ST7735_SWRESET, DELAY, //  1: Software reset, 0 args, w/delay
        150,                   //     150 ms delay
        ST7735_SLPOUT, DELAY,  //  2: Out of sleep mode, 0 args, w/delay
        255,                   //     500 ms delay
        ST7735_FRMCTR1, 3,     //  3: Frame rate ctrl - normal mode, 3 args:
        0x01, 0x2C, 0x2D,      //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3,     //  4: Frame rate control - idle mode, 3 args:
        0x01, 0x2C, 0x2D,      //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6,     //  5: Frame rate ctrl - partial mode, 6 args:
        0x01, 0x2C, 0x2D,      //     Dot inversion mode
        0x01, 0x2C, 0x2D,      //     Line inversion mode
        ST7735_INVCTR, 1,      //  6: Display inversion ctrl, 1 arg, no delay:
        0x07,                  //     No inversion
        ST7735_PWCTR1, 3,      //  7: Power control, 3 args, no delay:
        0xA2,
        0x02,                         //     -4.6V
        0x84,                         //     AUTO mode
        ST7735_PWCTR2, 1,             //  8: Power control, 1 arg, no delay:
        0xC5,                         //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        ST7735_PWCTR3, 2,             //  9: Power control, 2 args, no delay:
        0x0A,                         //     Opamp current small
        0x00,                         //     Boost frequency
        ST7735_PWCTR4, 2,             // 10: Power control, 2 args, no delay:
        0x8A,                         //     BCLK/2, Opamp current small & Medium low
        0x2A, ST7735_PWCTR5, 2,       // 11: Power control, 2 args, no delay:
        0x8A, 0xEE, ST7735_VMCTR1, 1, // 12: Power control, 1 arg, no delay:
        0x0E, ST7735_INVOFF, 0,       // 13: Don't invert display, no args, no delay
        ST7735_MADCTL, 1,             // 14: Memory access control (directions), 1 arg:
        0xC8,                         //     row addr/col addr, bottom to top refresh
        ST7735_COLMOD, 1,             // 15: set color mode, 1 arg, no delay:
        0x05},                        //     16-bit color

    Rcmd2green[] = {        // Init for 7735R, part 2 (green tab only)
        2,                  //  2 commands in list:
        ST7735_CASET, 4,    //  1: Column addr set, 4 args, no delay:
        0x00, 0x02,         //     XSTART = 0
        0x00, 0x7F + 0x02,  //     XEND = 127
        ST7735_RASET, 4,    //  2: Row addr set, 4 args, no delay:
        0x00, 0x01,         //     XSTART = 0
        0x00, 0x9F + 0x01}, //     XEND = 159
    Rcmd2red[] = {          // Init for 7735R, part 2 (red tab only)
        2,                  //  2 commands in list:
        ST7735_CASET, 4,    //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,         //     XSTART = 0
        0x00, 0x7F,         //     XEND = 127
        ST7735_RASET, 4,    //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,         //     XSTART = 0
        0x00, 0x9F},        //     XEND = 159

    Rcmd2green144[] = {  // Init for 7735R, part 2 (green 1.44 tab)
        2,               //  2 commands in list:
        ST7735_CASET, 4, //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,      //     XSTART = 0
        0x00, 0x7F,      //     XEND = 127
        ST7735_RASET, 4, //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,      //     XSTART = 0
        0x00, 0x7F},     //     XEND = 127

    Rcmd2green160x80[] = { // 7735R init, part 2 (mini 160x80)
        2,                 //  2 commands in list:
        ST7735_CASET, 4,   //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,        //     XSTART = 0
        0x00, 0x4F,        //     XEND = 79
        ST7735_RASET, 4,   //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,        //     XSTART = 0
        0x00, 0x9F},       //     XEND = 159

    Rcmd3[] = {                                                                                                              // Init for 7735R, part 3 (red or green tab)
        4,                                                                                                                   //  4 commands in list:
        ST7735_GMCTRP1, 16,                                                                                                  //  1: Magical unicorn dust, 16 args, no delay:
        0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, ST7735_GMCTRN1, 16,  //  2: Sparkles and rainbows, 16 args, no delay:
        0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, ST7735_NORON, DELAY, //  3: Normal display on, no args, w/delay
        10,                                                                                                                  //     10 ms delay
        ST7735_DISPON, DELAY,                                                                                                //  4: Main screen turn on, no args w/delay
        100};                                                                                                                //     100 ms delay

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in byte array.
void Adafruit_ST7735_Mini::commandList(uint8_t *addr)
{

    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++; // Number of commands to follow
    while (numCommands--)
    {                          // For each command...
        writecommand(*addr++); //   Read, issue command
        numArgs = *addr++;     //   Number of args to follow
        ms = numArgs & DELAY;  //   If hibit set, delay follows args
        numArgs &= ~DELAY;     //   Mask out delay bit
        while (numArgs--)
        {                       //   For each argument...
            writedata(*addr++); //     Read, issue argument
        }

        if (ms)
        {
            ms = *addr++; // Read post-command delay time (ms)
            if (ms == 255)
                ms = 500; // If 255, delay for 500 ms
            ThisThread::sleep_for(std::chrono::milliseconds(ms));
        }
    }
}

// Initialization code common to both 'B' and 'R' type displays
void Adafruit_ST7735_Mini::commonInit(uint8_t *cmdList)
{

    colstart = rowstart = 0; // May be overridden in init func

    _rs = 1;
    _cs = 1;

    // use default SPI format
    lcdPort.format(8, 0);

    // toggle RST low to reset; CS low so it'll listen to us
    _cs = 0;
    _rst = 1;
    ThisThread::sleep_for(std::chrono::milliseconds(500));
    _rst = 0;
    ThisThread::sleep_for(std::chrono::milliseconds(500));
    _rst = 1;
    ThisThread::sleep_for(std::chrono::milliseconds(500));

    if (cmdList)
        commandList(cmdList);
}

// Initialization for ST7735B screens
void Adafruit_ST7735_Mini::initB(void)
{
    commonInit(Bcmd);
}

// Initialization for ST7735R screens (green or red tabs)
void Adafruit_ST7735_Mini::initR(uint8_t options, int8_t colshift, int8_t rowshift)
{
    commonInit(Rcmd1);
    if (options == INITR_GREENTAB)
    {
        commandList(Rcmd2green);
        colstart = 2 + colshift;
        rowstart = 1 + rowshift;
    }
    else if ((options == INITR_144GREENTAB) || (options == INITR_HALLOWING))
    {
        _height = ST7735_TFTHEIGHT_128;
        _width = ST7735_TFTWIDTH_128;
        commandList(Rcmd2green144);
        colstart = 2 + colshift;
        rowstart = 3 + rowshift; // For default rotation 0
    }
    else if (options == INITR_MINI160x80)
    {
        _height = ST7735_TFTWIDTH_80;
        _width = ST7735_TFTHEIGHT_160;
        commandList(Rcmd2green160x80);
        colstart = 24 + colshift;
        rowstart = 0 + rowshift;
    }
    else
    {
        // colstart, rowstart left at default '0' values
        commandList(Rcmd2red);
    }
    commandList(Rcmd3);

    // Black tab, change MADCTL color filter
    if ((options == INITR_BLACKTAB) || (options == INITR_MINI160x80))
    {
        uint8_t data = 0xC0;
        writecommand(ST7735_MADCTL);
        writedata(data);
    }

    if (options == INITR_HALLOWING)
    {
        // Hallowing is simply a 1.44" green tab upside-down:
        tabcolor = INITR_144GREENTAB;
        setRotation(2);
    }
    else
    {
        tabcolor = options;
        setRotation(0);
    }
}

void Adafruit_ST7735_Mini::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,
                                         uint8_t y1)
{
    writecommand(ST7735_CASET); // Column addr set
    writedata(0x00);
    writedata(x0 + colstart); // XSTART
    writedata(0x00);
    writedata(x1 + colstart); // XEND

    writecommand(ST7735_RASET); // Row addr set
    writedata(0x00);
    writedata(y0 + rowstart); // YSTART
    writedata(0x00);
    writedata(y1 + rowstart); // YEND

    writecommand(ST7735_RAMWR); // write to RAM
}

void Adafruit_ST7735_Mini::pushColor(uint16_t color)
{
    _rs = 1;
    _cs = 0;

    lcdPort.write(color >> 8);
    lcdPort.write(color);
    _cs = 1;
}

void Adafruit_ST7735_Mini::drawPixel(int16_t x, int16_t y, uint16_t color)
{

    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
        return;

    setAddrWindow(x, y, x + 1, y + 1);

    _rs = 1;
    _cs = 0;

    lcdPort.write(color >> 8);
    lcdPort.write(color);

    _cs = 1;
}

void Adafruit_ST7735_Mini::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                         uint16_t color)
{

    // Rudimentary clipping
    if ((x >= _width) || (y >= _height))
        return;
    if ((y + h - 1) >= _height)
        h = _height - y;
    setAddrWindow(x, y, x, y + h - 1);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    while (h--)
    {
        lcdPort.write(hi);
        lcdPort.write(lo);
    }
    _cs = 1;
}

void Adafruit_ST7735_Mini::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                         uint16_t color)
{

    // Rudimentary clipping
    if ((x >= _width) || (y >= _height))
        return;
    if ((x + w - 1) >= _width)
        w = _width - x;
    setAddrWindow(x, y, x + w - 1, y);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    while (w--)
    {
        lcdPort.write(hi);
        lcdPort.write(lo);
    }
    _cs = 1;
}

void Adafruit_ST7735_Mini::fillScreen(uint16_t color)
{
    fillRect(0, 0, _width, _height, color);
}

void Adafruit_ST7735_Mini::fillFastScreen(uint16_t color, char* buffer, size_t buffer_size)
{
    fillFastRect(0, 0, _width, _height, color, buffer, buffer_size);
}

// fill a rectangle
void Adafruit_ST7735_Mini::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                    uint16_t color)
{
    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height))
        return;
    if ((x + w - 1) >= _width)
        w = _width - x;
    if ((y + h - 1) >= _height)
        h = _height - y;

    setAddrWindow(x, y, x + w - 1, y + h - 1);

    uint8_t hi = color >> 8, lo = color;
    _rs = 1;
    _cs = 0;
    for (y = h; y > 0; y--)
    {
        for (x = w; x > 0; x--)
        {
            lcdPort.write(hi);
            lcdPort.write(lo);
        }
    }

    _cs = 1;
}

void Adafruit_ST7735_Mini::fillFastRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, 
    char* buffer, size_t buffer_size)
{
    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width) w = _width - x;
    if ((y + h - 1) >= _height) h = _height - y;

    // Prepare buffer
    if ((int)buffer_size < 2 * w) return;
    int buffer_height = min(h, (int16_t)(buffer_size / (w * 2)));
    buffer_size = buffer_height * w * 2;
    for (int i = 0; i < (int)buffer_size - 1; i += 2)
    {
        buffer[i] = color >> 8;
        buffer[i + 1] = color;
    }

    // Write slices
    int s = 0;
    for (s = 0; s <= h - buffer_height; s += buffer_height)
    {
        setAddrWindow(x, y + s, x + w - 1, y + s + buffer_height - 1);
        _rs = 1;
        _cs = 0;
        lcdPort.write(buffer, buffer_size, 0, 0);
        _cs = 1;
    }

    // Write rest
    setAddrWindow(x, y + s, x + w - 1, y + h - 1);
    _rs = 1;
    _cs = 0;
    lcdPort.write(buffer, (h - s) * w * 2, 0, 0);
    _cs = 1;
}

// display a bitmap
void Adafruit_ST7735_Mini::drawFastBitmap(int16_t x, int16_t y,
    const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg_color, char* buffer, size_t buffer_size, bool msb)
{
    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height)) return;
    // Keep original width for bitmap clipping
    int16_t og_w = w;
    int16_t byte_width = (og_w + 7) / 8; 
    if ((x + w - 1) >= _width) w = _width - x;
    // Bitmap scanline pad = whole byte
    if ((y + h - 1) >= _height) h = _height - y;

    // Prepare buffer
    if ((int)buffer_size < 2 * w) return;
    int buffer_height = min(h, (int16_t)(buffer_size / (w * 2)));
    buffer_size = buffer_height * w * 2;

    // Write slices
    int s = 0;
    for (s = 0; s <= h - buffer_height; s += buffer_height)
    {
        // Generate slice
        int buffer_index = 0;
        for (int16_t j = s; j < s + buffer_height; j++) for (int16_t i = 0; i < og_w; i++) 
        {
            // Select correct byte from source bitmap
            uint8_t source_byte = 0;
            if (msb) 
            {
                source_byte = bitmap[(j * byte_width) + (i / 8)];
            }
            else 
            {
                source_byte = bitmap[((j + 1) * byte_width) - (i / 8) - 1];
            }
            // Select correct bit from source byte
            uint16_t current_color = (source_byte & (1 << (7 - (i & 7))))? color : bg_color;
            if(i < w)
            {
                buffer[buffer_index] = current_color >> 8;
                buffer[buffer_index + 1] = current_color;
                buffer_index += 2;
            }
        }

        // Output slice
        setAddrWindow(x, y + s, x + w - 1, y + s + buffer_height - 1);
        _rs = 1;
        _cs = 0;
        lcdPort.write(buffer, buffer_size, 0, 0);
        _cs = 1;
    }

    // Generate rest
    int buffer_index = 0;
    for(int16_t j = s; j < h; j++) for(int16_t i = 0; i < og_w; i++) 
    {
        uint16_t current_color = (bitmap[(j * byte_width) + (i / 8)] & (1 << (7 - (i & 7))))? color : bg_color;
        if(i < w)
        {
            buffer[buffer_index] = current_color >> 8;
            buffer[buffer_index + 1] = current_color;
            buffer_index += 2;
        }
    }

    // Output rest
    setAddrWindow(x, y + s, x + w - 1, y + h - 1);
    _rs = 1;
    _cs = 0;
    lcdPort.write(buffer, (h - s) * w * 2, 0, 0);
    _cs = 1;
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ST7735_Mini::Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

void Adafruit_ST7735_Mini::setRotation(uint8_t m)
{
    uint8_t madctl = 0;

    rotation = m & 3; // can't be higher than 3

    // For ST7735 with GREEN TAB (including HalloWing)...
    if ((tabcolor == INITR_144GREENTAB) || (tabcolor == INITR_HALLOWING))
    {
        // ..._rowstart is 3 for rotations 0&1, 1 for rotations 2&3
        rowstart = (rotation < 2) ? 3 : 1;
    }

    switch (rotation)
    {
    case 0:
        if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80))
        {
            madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
        }
        else
        {
            madctl = MADCTL_MX | MADCTL_MY | MADCTL_BGR;
        }

        if (tabcolor == INITR_144GREENTAB)
        {
            _height = ST7735_TFTHEIGHT_128;
            _width = ST7735_TFTWIDTH_128;
        }
        else if (tabcolor == INITR_MINI160x80)
        {
            _height = ST7735_TFTHEIGHT_160;
            _width = ST7735_TFTWIDTH_80;
        }
        else
        {
            _height = ST7735_TFTHEIGHT_160;
            _width = ST7735_TFTWIDTH_128;
        }
        break;
    case 1:
        if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80))
        {
            madctl = MADCTL_MY | MADCTL_MV | MADCTL_RGB;
        }
        else
        {
            madctl = MADCTL_MY | MADCTL_MV | MADCTL_BGR;
        }

        if (tabcolor == INITR_144GREENTAB)
        {
            _width = ST7735_TFTHEIGHT_128;
            _height = ST7735_TFTWIDTH_128;
        }
        else if (tabcolor == INITR_MINI160x80)
        {
            _width = ST7735_TFTHEIGHT_160;
            _height = ST7735_TFTWIDTH_80;
        }
        else
        {
            _width = ST7735_TFTHEIGHT_160;
            _height = ST7735_TFTWIDTH_128;
        }
        break;
    case 2:
        if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80))
        {
            madctl = MADCTL_RGB;
        }
        else
        {
            madctl = MADCTL_BGR;
        }

        if (tabcolor == INITR_144GREENTAB)
        {
            _height = ST7735_TFTHEIGHT_128;
            _width = ST7735_TFTWIDTH_128;
        }
        else if (tabcolor == INITR_MINI160x80)
        {
            _height = ST7735_TFTHEIGHT_160;
            _width = ST7735_TFTWIDTH_80;
        }
        else
        {
            _height = ST7735_TFTHEIGHT_160;
            _width = ST7735_TFTWIDTH_128;
        }
        break;
    case 3:
        if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80))
        {
            madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
        }
        else
        {
            madctl = MADCTL_MX | MADCTL_MV | MADCTL_BGR;
        }

        if (tabcolor == INITR_144GREENTAB)
        {
            _width = ST7735_TFTHEIGHT_128;
            _height = ST7735_TFTWIDTH_128;
        }
        else if (tabcolor == INITR_MINI160x80)
        {
            _width = ST7735_TFTHEIGHT_160;
            _height = ST7735_TFTWIDTH_80;
        }
        else
        {
            _width = ST7735_TFTHEIGHT_160;
            _height = ST7735_TFTWIDTH_128;
        }
        break;
    }

    writecommand(ST7735_MADCTL);
    writedata(madctl);
}

void Adafruit_ST7735_Mini::invertDisplay(boolean i)
{
    writecommand(i ? ST7735_INVON : ST7735_INVOFF);
}
