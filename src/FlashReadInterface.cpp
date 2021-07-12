/**
 * @file FlashReadInterface.cpp
 * @author Christoph Honal
 * @brief Provides the entrypoint for the experimental flash reader
 * @version 0.1
 * @date 2021-07-12
 */

#ifdef FLASH_READ_INTERFACE

    #include <mbed.h>
    #include <pinmap_ex.h>

    #include "GT24L24A2Y.h"
    #include "Adafruit_ST7735_Mini.h"

    #include "CoreService.h"
    #include "HardwareConfiguration.h"


    #define LCD_BUFFER_SIZE 320 


    const PinMapSPI PinMap_SPI[] = 
    {
        { PIN_LCD_MOSI, NC, PIN_LCD_CLK, 1 },
        { PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, 2 },
        { NC, NC, NC, NC }
    }; //!< SPI interface map


    static GT24L24A2Y flash(PIN_FONT_MOSI, PIN_FONT_MISO, PIN_FONT_CLK, PIN_ACC_CS); //!< Flash
    static Adafruit_ST7735_Mini lcd(PIN_LCD_MOSI, NC, PIN_LCD_CLK, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RESET, LCD_SPI_FREQ);  //!< LCD
    static PwmOutLP lcd_bl(PIN_LCD_BL); //!< LCD backlight
    static DigitalOut lcd_pwr(PIN_LCD_PWR); //!< LCD power
    static char lcd_bitmap_buffer[LCD_BUFFER_SIZE]; //!< LCD buffer for fast drawing
    
    /**
     * @brief Main entrypoint for flash interface
     * 
     * @return int Return error code
     */
    int main()
    {
        // Setup LCD
        lcd.initR(INITR_MINI160x80, LCD_COLSHIFT, LCD_ROWSHIFT);
        lcd.setRotation(2);
        lcd.setCursor(0, 0);
        lcd.setTextSize(1);
        lcd.setTextWrap(false);
        lcd.setTextColor(LCD_COLOR_WHITE);
        lcd_bl.setPower(true);
        lcd_pwr.write(1);
        lcd_bl.write(1.0f);

        // Write message
        lcd.fillFastScreen(LCD_COLOR_BLACK, lcd_bitmap_buffer, LCD_BUFFER_SIZE);
        lcd.printf("Not impl.");

        // Soft halt
        while(true) ThisThread::sleep_for(1000);

        return 0;
    }

#endif
