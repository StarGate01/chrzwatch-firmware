/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
 
Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include <mbed.h>
#include <Stream.h>


#define aswap(a, b) { int16_t t = a; a = b; b = t; }
#define boolean bool

class Adafruit_GFX : public Stream
{
public:

    //Adafruit_GFX();
    Adafruit_GFX(int16_t w, int16_t h); // Constructor
    // i have no idea why we have to formally call the constructor. kinda sux
    //void constructor(int16_t w, int16_t h);

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void invertDisplay(boolean i);

    // these are 'generic' drawing functions, so we can share them!
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void fillFastRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, char* buffer, size_t buffer_size);
    virtual void fillScreen(uint16_t color);
    virtual void fillFastScreen(uint16_t color, char* buffer, size_t buffer_size);

    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

    virtual void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, 
        int16_t w, int16_t h, uint16_t color);
    virtual void drawFastBitmap(int16_t x, int16_t y, const uint8_t *bitmap, 
        int16_t w, int16_t h, uint16_t color, uint16_t bg_color, char* buffer, size_t buffer_size, bool msb = true);
    void drawChar(int16_t x, int16_t y, unsigned char c,
        uint16_t color, uint16_t bg, uint8_t size);

    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(boolean w);

    int16_t height(void);
    int16_t width(void);

    void setRotation(uint8_t r);
    uint8_t getRotation(void);

protected:
    virtual int _putc(int value);
    virtual int _getc();
    
    const int16_t  WIDTH, HEIGHT;   // this is the 'raw' display w/h - never changes
    int16_t  _width, _height; // dependent on rotation
    int16_t  cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    uint8_t  textsize;
    uint8_t  rotation;
    boolean  wrap; // If set, 'wrap' text at right edge of display
};

#endif
