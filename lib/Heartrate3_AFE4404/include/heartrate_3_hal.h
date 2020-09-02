/****************************************************************************
* Title                 :   HeartRate 3 click
* Filename              :   heartrate_3_hal.h
* Author                :   CAL
* Origin Date           :   04/04/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date     Software Version    Initials      Description
*  04/04/2016    XXXXXXXXXXX        CAL      Interface Created.
*
*****************************************************************************/
/**
 * @file heartrate_3_hal.h
 * @brief <h2> HAL layer </h2>
 *
 * @par
 * HAL layer for <a href="http://www.mikroe.com">MikroElektronika's</a>
 * Heart Rate 3 click board.
 */
#ifndef HEARTRATE_3_HAL_H_
#define HEARTRATE_3_HAL_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define CMD_SIZE 1    // COMMAND OR REGISTER ADDRESS SIZE
#define BUFF_SIZE 256 // BUFFER SIZE
/******************************************************************************
* Configuration Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Variables
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief <h3>HAL Initialization</h3>
     *
     * @par
     * Initialization of HAL layer used to initialize I2C bus and pins needed
     * for proper usage of click board.
     *
     * @param[in] address_id - hardware address
     */
    void hr3_hal_init(void* mbed_i2c, uint8_t address_id);

    /**
     * @brief <h3>HAL Write</h3>
     *
     * @par
     * Generic write function adapted for I2C bus.
     *
     * @param[in] command - register address
     * @param[in] buffer - data buffer
     * @param[in] count - data size in bytes
     */
    void hr3_hal_write(uint8_t *command, uint8_t *buffer, uint16_t count);

    /**
     * @brief <h3>HAL Read</h3>
     *
     * @par
     * Generic read function adapted for SHT click.
     *
     * @param[in] command - register address
     * @param[out] buffer - data buffer
     * @param[in] count - data size in bytes
     */
    void hr3_hal_read(uint8_t *command, uint8_t *buffer, uint16_t count);

    void hr3_hal_delay(uint32_t ms);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HEARTRATE_3_HAL_H_ */

/*** End of File **************************************************************/
