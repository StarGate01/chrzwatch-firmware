/**
 * @file HardwareConfiguration.h
 * @author Christoph Honal
 * @brief Defines configuration macros for pin mapping
 * @version 0.1
 * @date 2020-05-30
 */

#if defined(TARGET_VARIANT_I6HRC)
#   define PIN_VIBRATION    P0_25
#   define PIN_BUTTON1      P0_26
#   define PIN_BUTTON2      P0_27
#else
#   define PIN_VIBRATION    LED2
#   define PIN_BUTTON1      BUTTON1
#   define PIN_BUTTON2      BUTTON2
#endif