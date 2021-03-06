/**
 * @file UserSettings.h
 * @author Christoph Honal
 * @brief Defines the layout of user settings
 * @version 0.1
 * @date 2021-07-06
 */

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include <mbed.h>

/**
 * @brief Stores various sensor settings
 * 
 */
struct __attribute__ ((packed)) user_sensor_settings_t
{
    uint16_t step_length           = 75;  //!< Step distance used for speed estimate, in cm
    uint8_t motion_duration        = 25;  //!< Motion detection time window in 1/50 s
    uint8_t motion_threshold       = 8;   //!< Motion detection threshold in 1/16 g
    uint8_t cadence_running_thresh = 120; //!< Minimum step cadence for running
};

/**
 * @brief Stores various user settings19
 * 
 */
struct __attribute__ ((packed)) user_settings_t
{
    user_sensor_settings_t sensor; //!< Sensor settings
};

#endif
