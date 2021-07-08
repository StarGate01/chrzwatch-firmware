/**
 * @file HardwareConfiguration.h
 * @author Christoph Honal
 * @brief Defines configuration macros for pin mapping
 * @version 0.1
 * @date 2020-05-30
 */

#define TARGET_VARIANT_NAME  u8"CHRZwatch Unknown\0"
#define PIN_VIBRATION        P0_25
#define PIN_BUTTON1          P0_26
#define PIN_BUTTON2          P0_27
#define PIN_BATTERY          P0_28
#define PIN_CHARGE           P0_23
#define PIN_LCD_CS           P0_11
#define PIN_LCD_RESET        P0_12
#define PIN_LCD_DC           P0_13
#define PIN_LCD_SCL          P0_14
#define PIN_LCD_SDA          P0_15
#define PIN_LCD_BL           P0_4
#define PIN_LCD_PWR          P0_30
#define PIN_ACC_SDA          P0_3
#define PIN_ACC_SCL          P0_2
#define PIN_ACC_CS           P0_16
#define PIN_ACC_ADDR         P0_17
#define PIN_ACC_INT          P0_22
#define PIN_HR_PWR           P0_10
#define PIN_HR_ADCREADY      P0_18
#define PIN_HR_RESET         P0_19
#define PIN_HR_SDA           P0_20
#define PIN_HR_SCL           P0_21
#define PIN_FONT_MISO        P0_5
#define PIN_FONT_CS          P0_6
#define PIN_FONT_CLK         P0_7
#define PIN_FONT_MOSI        P0_8
#define PIN_RX               P0_9
#define PIN_TX               P0_10
#define THREAD_SIZE          2048 //!< Child thread stack size in bytes
#define LCD_COLSHIFT         0 //!< LCD pixel column shift
#define LCD_ROWSHIFT         0 //!< LCD pixel row shift
#define LCD_SPI_FREQ         8000000 //!< LCD SPI speed, 8Mhz is CPU max
#define BATTERY_MAX          0.4f //!< 100% battery voltage / 10 V
#define BATTERY_MIN          0.34f //!< 0% battery voltage / 10 V

#if defined(TARGET_VARIANT_I6HRC)
#   define TARGET_VARIANT_NAME  u8"CHRZwatch I6HRC\0"
#elif defined(TARGET_VARIANT_NRF52_DK)
#   define TARGET_VARIANT_NAME  u8"CHRZwatch NRF52_DK\0"
#   define PIN_VIBRATION        LED2
#   define PIN_BUTTON1          BUTTON1
#   define PIN_BUTTON2          BUTTON2  
#   define PIN_CHARGE           BUTTON3
#   define PIN_VIBRATION_INVERT 1
#   define LCD_COLSHIFT         2
#   define LCD_ROWSHIFT         1
#   define LCD_INVERT           1
#endif