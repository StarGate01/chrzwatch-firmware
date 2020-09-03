/****************************************************************************
* Title                 :   HeartRate 3 Click
* Filename              :   heartrate_3_hw.h
* Author                :   Corey Lakey
* Origin Date           :   04/04/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version     Initials    Description
*  04/04/2016         1.0           CAL
*
*****************************************************************************/
/** 
 * @file heartrate_3_hw.h
 *  @brief Hardware interface for heartrate1 or Heartrate 1 Click
 *
 *  This driver supports I2C bus communication as well as full featured
 *  access to the registers.
 *
 * @mainpage
 * @section General Description
 * @par
 * The heartrate 3 is an integrated pulse oximetry and heartrate monitor sensor
 * solution. It combines two LEDs, a photodetector, optimized optics,
 * and low-noise analog signal processing to detect pulse oximetry and
 * heart-rate signals.
 * The heartrate 3 operates from 1.8V and 3.3V power supplies and can be
 * powered down through software with negligible standby current,
 * permitting the power supply to remain connected at all times.
 *
 *  @date 06 Apr 2016
 *  @author Corey Lakey
 *  @copyright MikroElektronika
 *
 *  @version .1 - Initial testing and verification
 *
 *  @note Test configuration:
 *   MCU:             STM32F107VC
 *   Dev.Board:       EasyMx Pro v7
 *   Oscillator:      72 Mhz internal
 *   Ext. Modules:    Heartrate1_Click
 *   SW:              ARM 4.5.2
 *
 *  @ref specifications
 */
#ifndef HEARTRATE_3_HW_H_
#define HEARTRATE_3_HW_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Configuration Constants
*******************************************************************************/
#define MAX30100_I2C_ADR 0x57

//Diagnosis
#define DIAGNOSIS 0x00
#define DIAG_SW_RST 3     /**< For software reset, write 1              */
#define DIAG_EN 2         /**< 0: No action 1: Diagnostic mode enabled  */
#define DIAG_TM_CNT_RST 1 /**< Used to suspend count/keep counter reset */
#define DIAG_REG_READ 0   /**< 0: Register Write Mode 1: Enable readout */

//PRPCT ( timer counter )
#define PRPCT 0x1D /**< Bits 0-15 for writing counter value      */

//Timer Module enable / NUMAV ( # of times to sample and average )
#define TIM_NUMAV 0x1E
#define NUMAV 0   /**< Starts at bit 0, no bit shifting needed  */
#define TIMEREN 0 /**< 0: Module Disabled 1: Enables Module     */
                  // TIMEREN is on byte 2 and NUMAV is on byte 1

//TIA Gains 1
#define TIA_GAINS1 0x21
#define TIA_PROG_TG_EN 1 /**< Replaces ADC_RDY with programmable signal*/
#define TIA_CF 3         /**< Control of C(1) Setting                  */
#define TIA_GAIN 0       /**< Control of R(1) Setting                  */
                         //If ENSEPGAIN = 0, TIA_CF and TIA_GAIN control
                         //TIA_CF controls C1 and C2 Settings and
                         //TIA_GAIN controls R1 and R2 Settings

//TIA Gains 2
#define TIA_GAINS2 0x20
#define TIA_ENSEPGAIN 8 /**< 0: Single TIA Gain 1: Two sets TIA gains */
#define TIA_CF_SEP 3    /**< Control of C(2) Setting                  */
#define TIA_GAIN_SEP 0  /**< Control of R(2) Setting                  */

//LED1 Start / End
#define LED1_ST 0x03
#define LED1_END 0x04

//Sample LED1 Start / End
#define SMPL_LED1_ST 0x07
#define SMPL_LED1_END 0x08

//LED1 Convert Start / End
#define LED1_CONV_ST 0x11
#define LED1_CONV_END 0x12

//Sample Ambient 1 Start / End
#define SMPL_AMB1_ST 0x0B
#define SMPL_AMB1_END 0x0C

//Ambient 1 Convert Start / End
#define AMB1_CONV_ST 0x13
#define AMB1_CONV_END 0x14

//LED2 Start / End
#define LED2_ST 0x09
#define LED2_END 0x0A

//Sample LED2 Start / End
#define SMPL_LED2_ST 0x01
#define SMPL_LED2_END 0x02

//LED2 Convert Start / End
#define LED2_CONV_ST 0x0D
#define LED2_CONV_END 0x0E

//Sample Ambient 2 ( or LED3 ) Start / End
#define SMPL_LED3_ST 0x05
#define SMPL_LED3_END 0x06

//Ambient 2 ( or LED3 ) Convert Start / End
#define LED3_CONV_ST 0x0F
#define LED3_CONV_END 0x10

//ADC Reset Phase 0 Start / End
#define ADC_RST_P0_ST 0x15
#define ADC_RST_P0_END 0x16

//ADC Reset Phase 1 Start / End
#define ADC_RST_P1_ST 0x17
#define ADC_RST_P1_END 0x18

//ADC Reset Phase 2 Start / End
#define ADC_RST_P2_ST 0x19
#define ADC_RST_P2_END 0x1A

//ADC Reset Phase 3 Start / End
#define ADC_RST_P3_ST 0x1B
#define ADC_RST_P3_END 0x1C

//LED Current Control
#define LED_CONFIG 0x22
/* ********************************************************************
 * LED current control is a 24 bit register where                     *
 * LED1: bits 0-5 LED2: bits 6-11 LED3: bits 12-17 and the rest are 0 *
 * ****************************************************************** *
 * LED1, LED2, LED3 Register Values | LED Current Setting (mA)        *
 *                   0              |              0                  *
 *                   1              |             0.8                 *
 *                   2              |             1.6                 *
 *                   3              |             2.4                 *
 *                  ...             |             ...                 *
 *                   63             |              50                 *
 **********************************************************************/

//Dynamic Settings Address + registers

#define SETTINGS 0x23  /**< Settings Address */
#define STT_DYNMC1 20  /**< 0: Transmitter not pwrd dwn 1: pwrd dwn  */
#define STT_ILED_2X 17 /**< 0: LED crrnt range 0-50 1: range 0-100   */
#define STT_DYNMC2 14  /**< 0: ADC not pwrd dwn 1: ADC pwrd dwn      */
#define STT_OSC_EN 9   /**< 0: External Clock 1: Internal Clock      */
#define STT_DYNMC3 4   /**< 0: TIA not pwrd dwn 1: TIA pwrd dwn      */
#define STT_DYNMC4 3   /**< 0: Rest of ADC ! pwrd dwn 1: Is pwrd dwn */
#define STT_PDNRX 1    /**< 0: Normal Mode 1: RX of AFE pwrd dwn     */
#define STT_PDNAFE 0   /**< 0: Normal Mode 1: Entire AFE pwrd dwn    */

//Clockout Settings
#define CLKOUT 0x29  /**< Clockout Address */
#define CLKOUT_EN 1  /**< 0: Disable clock output 1: Enable output */
#define CLKOUT_DIV 1 /**< Frequency of clock output on CLK pin     */
/* *****************************************************************************
  *  CLKOUT_DIV Register Settings Graph
  * ****************************************************************************
  * CLKOUT_DIV Register Settings | Division Ratio | Output Clock Freq. ( MHz ) *
  *             0                |         1      |             4              *
  *             1                |         2      |             2              *
  *             2                |         4      |             1              *
  *             3                |         8      |            0.5             *
  *             4                |        16      |           0.25             *
  *             5                |        32      |          0.125             *
  *             6                |        64      |         0.0625             *
  *             7                |       128      |        0.03125             *
  *         8..15                |    Do not use  |      Do not use            *
  *****************************************************************************/

//LED1 Output Value
#define LED1VAL 0x2C /**< LED1 Output code in twos complement      */

//LED2 Output Value
#define LED2VAL 0x2A /**< LED2 Output code in twos complement      */

//Ambient 1 Value
#define ALED1VAL 0x2D /**< Ambient 1 value in twos complement       */

//LED1-Ambient 1 Value
#define LED1_ALED1VAL 0x2F /**< LED1-ambient1 in twos complement         */

//LED2-Ambient 2 Value
#define LED2_ALED2VAL 0x2E /**< LED2-ambient2 in twos complement         */

//LED3 or Ambient 2 Value
#define LED3VAL 0x2B /**< LED3 / Ambient 2 value in twos complement*/

//Diagnostics Flag
#define PD_SHORT_FLAG 0x30 /**< 0: No short across PD 1: Short across PD */

//PD disconnect / INP, INN settings / EXT clock Division settings
#define PD_INP_EXT 0x31
#define PD_DISCONNECT 2      /**< Disconnects PD signals (INP, INM)        */
#define ENABLE_INPUT_SHORT 5 /**< INP, INN are shorted to VCM when TIA dwn */
#define CLKDIV_EXTMODE 0     /**< Ext Clock Div Ration bits 0-2            */

/* *****************************************************************************
  *  CLKDIV_EXTMODE Register Settings Graph
  * ****************************************************************************
  * CLKDIV_EXTMODE Settings      | Division Ratio | Allowed Clock Freq ( MHz ) *
  *             0                |         2      |           8-12             *
  *             1                |         8      |          32-48             *
  *             2                |    Do Not Use  |       Do Not Use           *
  *             3                |         8      |          48-60             *
  *             4                |        16      |          16-24             *
  *             5                |        32      |            4-6             *
  *             6                |        64      |          24-36             *
  *             7                |    Do Not Use  |       Do Not Use           *
  *****************************************************************************/

//PDN_CYCLE Start / End
#define PDNCYCLESTC 0x32  /**< Bits 0-15                                */
#define PDNCYCLEENDC 0x33 /**< Bits 0-15                                */

//Programmable Start / End time for ADC_RDY replacement
#define PROG_TG_STC 0x34  /**< Bits 0-15 Define Start Time              */
#define PROG_TG_ENDC 0x35 /**< Bits 0-15 Define End Time                */

//LED3C Start / End
#define LED3LEDSTC 0x36  /**< LED3 Start, if not used set to 0         */
#define LED3LEDENDC 0x37 /**< LED3 End, if not used set to 0           */

//PRF Clock Division settings
#define CLKDIV_PRF 0x39 /**< Clock Division Ratio for timing engine   */
/* ****************************************************************************
  *   CLKDIV_PRF Register Settings Graph
  * ****************************************************************************
  * CLKDIV_PRF Settings|Division Ratio|Clock Freq(MHz)| Lowest PRF Setting (Hz)*
  *          0         |       1      |       4       |           61           *
  *          1         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          2         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          3         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          4         |       2      |       2       |           31           *
  *          5         |       4      |       1       |           15           *
  *          6         |       8      |     0.5       |            8           *
  *          7         |      16      |    0.25       |            4           *
  *****************************************************************************/

//DAC Settings
#define DAC_SETTING 0x3A   /**< DAC Settings Address                     */
#define POL_OFFDAC_LED2 19 /**< Polarity for LED2                        */
#define I_OFFDAC_LED2 15   /**< Setting for LED2                         */
#define POL_OFFDAC_AMB1 14 /**< Polarity for Ambient 1                   */
#define I_OFFDAC_AMB1 10   /**< Setting for Ambient 1                    */
#define POL_OFFDAC_LED1 9  /**< Polarity for LED1                        */
#define I_OFFDAC_LED1 5    /**< Setting for LED1                         */
#define POL_OFFDAC_LED3 4  /**< Polarity for LED3                        */
#define I_OFFDAC_LED3 0    /**< Setting for LED3                         */
/* **************************************************************
 *  I_OFFDAC Register Settings                                  *
 * **************************************************************
 * * Reg. Settings | Offset Cancellation | Offset Cancellation  |
 * *               |   POL_OFFDAC = 0    |   POL_OFFDAC = 1     |
 * **************************************************************
 *        0        |          0          |           0          *
 *        1        |       0.47          |       –0.47          *
 *        2        |       0.93          |       –0.93          *
 *        3        |        1.4          |        –1.4          *
 *        4        |       1.87          |       –1.87          *
 *        5        |       2.33          |       –2.33          *
 *        6        |        2.8          |        –2.8          *
 *        7        |       3.27          |       –3.27          *
 *        8        |       3.73          |       –3.73          *
 *        9        |        4.2          |        –4.2          *
 *       10        |       4.67          |       –4.67          *
 *       11        |       5.13          |       –5.13          *
 *       12        |        5.6          |        –5.6          *
 *       13        |       6.07          |       –6.07          *
 *       14        |       6.53          |       –6.53          *
 *       15        |          7          |          –7          *
 ****************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

typedef enum
{
    sw_reset_dis = 0,
    sw_reset_en
} sw_reset_t;

typedef enum
{
    diag_mode_dis = 0,
    diag_mode_en
} diag_mode_t;

typedef enum
{
    susp_count_dis = 0,
    susp_count_en
} susp_count_t;

typedef enum
{
    reg_read_dis = 0,
    reg_read_en
} reg_read_t;

typedef enum
{
    trans_dis = 0,
    trans_en
} transmitter_t;

typedef enum
{
    led_norm = 0,
    led_double
} led_current_range_t;

typedef enum
{
    adc_off = 0,
    adc_on
} adc_pwer_t;

typedef enum
{
    ext_clk_mode = 0,
    osc_mode
} clk_mode_t;

typedef enum
{
    tia_on = 0,
    tia_off
} tia_pwer_t;

typedef enum
{
    rest_of_adc_off = 0,
    rest_of_adc_on
} rest_of_adc_t;

typedef enum
{
    afe_rx_normal = 0,
    afe_rx_off
} afe_rx_mode_t;

typedef enum
{
    afe_normal = 0,
    afe_off
} afe_mode_t;

typedef enum
{
    pd_connect = 0,
    pd_disconnect
} pd_setting_t;

typedef enum
{
    no_inm_short = 0,
    inm_short
} input_short_t;

typedef struct
{
    transmitter_t transmit;
    led_current_range_t curr_range;
    adc_pwer_t adc_power;
    clk_mode_t clk_mode;
    tia_pwer_t tia_power;
    rest_of_adc_t rest_of_adc;
    afe_rx_mode_t afe_rx_mode;
    afe_mode_t afe_mode;
} dynamic_modes_t;

typedef struct
{
    pd_setting_t pd_setting;
    input_short_t short_setting;
    uint8_t ext_div;
} inm_inn_t;

typedef struct
{
    uint8_t offdac_pol_led2;
    uint8_t offdac_set_led2;
    uint8_t offdac_pol_amb1;
    uint8_t offdac_set_amb1;
    uint8_t offdac_pol_led1;
    uint8_t offdac_set_led1;
    uint8_t offdac_pol_amb2;
    uint8_t offdac_set_amb2;
} offdac_settings_t;

/******************************************************************************
* Variables
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/**
 * @brief <h3> Set Settings </h3>
 *
 * @par
 *   Prepares settings
 *
 *
 * @param sw_reset      -   Software Reset pin enable / disable
 * @param diag_mode     -   Diagnostics mode enable / disable
 * @param susp_counter  -   Suspend Counter enable / disable
 * @param reg_read      -   Register read enable / disable ( disable on init )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_settings(sw_reset_t sw_reset, diag_mode_t diag_mode,
                        susp_count_t susp_counter, reg_read_t reg_read);

/**
 * @brief <h3> Set LED2 Sample Start and End </h3>
 *
 * @par
 *   Sets the LED2 sample start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED sample start ( time )
 * @param end        -   LED sample end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led2_sample_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED1 Start and End </h3>
 *
 * @par
 *   Sets the LED1 start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED start ( time )
 * @param end        -   LED end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led1_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED3 Sample Start and End </h3>
 *
 * @par
 *   Sets the LED3 sample start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED sample start ( time )
 * @param end        -   LED sample end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led3_sample_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED1 Sample Start and End </h3>
 *
 * @par
 *   Sets the LED1 sample start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED sample start ( time )
 * @param end        -   LED sample end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led1_sample_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED2 Start and End </h3>
 *
 * @par
 *   Sets the LED2 start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED start ( time )
 * @param end        -   LED end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led2_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set Ambient 1 Sample Start and End </h3>
 *
 * @par
 *   Sets the Ambient 1 sample start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   Ambient sample start ( time )
 * @param end        -   Ambient sample end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_amb1_sample_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED2 Convert Start and End </h3>
 *
 * @par
 *   Sets the LED2 conversion start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED convert start ( time )
 * @param end        -   LED convert end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led2_convert_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED3 Convert Start and End </h3>
 *
 * @par
 *   Sets the LED3 conversion start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED convert start ( time )
 * @param end        -   LED convert end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led3_convert_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED1 Convert Start and End </h3>
 *
 * @par
 *   Sets the LED1 conversion start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED convert start ( time )
 * @param end        -   LED convert end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led1_convert_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set Ambient 1 Convert Start and End </h3>
 *
 * @par
 *   Sets the Ambient 1 conversion start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   Ambient convert start ( time )
 * @param end        -   Ambient convert end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_amb1_convert_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set ADC Reset Phase 0 Start and End </h3>
 *
 * @par
 *   Sets the ADC start time and end time for phase 0. ( See Datasheet )
 *
 *
 * @param start      -   adc reset phase 0 start ( time )
 * @param end        -   adc reset phase 0 end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_adc_reset0_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set ADC Reset Phase 1 Start and End </h3>
 *
 * @par
 *   Sets the ADC start time and end time for phase 1. ( See Datasheet )
 *
 *
 * @param start      -   adc reset phase 1 start ( time )
 * @param end        -   adc reset phase 1 end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_adc_reset1_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set ADC Reset Phase 2 Start and End </h3>
 *
 * @par
 *   Sets the ADC start time and end time for phase 2. ( See Datasheet )
 *
 *
 * @param start      -   adc reset phase 2 start ( time )
 * @param end        -   adc reset phase 2 end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_adc_reset2_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set ADC Reset Phase 3 Start and End </h3>
 *
 * @par
 *   Sets the ADC start time and end time for phase 3. ( See Datasheet )
 *
 *
 * @param start      -   adc reset phase 3 start ( time )
 * @param end        -   adc reset phase 3 end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_adc_reset3_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set PRPCT Count </h3>
 *
 * @par
 *   The pulse repetition frequency (PRF) can be set using
 * the PRPCT register bits that represent the high value of the counter
 * (the low value of the counter is 0). The counter automatically counts until
 * reaching PRPCT and then returns to 0 to start the next count.
 *
 *
 * @param count -   High byte of counter for the pulse repetition freq. (PRF)
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_prpct_count(uint16_t count);

/**
 * @brief <h3> Set Timer enable and NUMAV ( Number of ADC averages ) </h3>
 *
 * @par
 *   To reduce the noise, the input to the ADC (sampled on the CSAMPx
 * capacitors) can be converted by the ADC multiple times and averaged.
 * The number of averages is set using the NUMAV register control
 *
 *
 * @param enable    -   Enable or Disable Timer Engine
 * @param av_num    -   Number of times ADC is averaged ( 0 - 15 ).
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_timer_and_average_num(bool enable, uint8_t av_num);

/**
 * @brief <h3> Set Seperate TIA Gain and CF Settings </h3>
 *
 * @par
 *   The signal current from the photodiode is converted to a differential
 * voltage using a transimpedance amplifier (TIA). Here you can set the gain and
 * cf settings for this seperate amplifier.
 *
 *
 * @param seperate      -   Enable or Disable using seperate amplifier
 * @param cf_setting    -   CF setting for seperate TIA
 * @param gain_setting  -   Gain setting for seperate TIA
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_seperate_tia_gain(bool seperate, uint8_t cf_setting, uint8_t gain_setting);

/**
 * @brief <h3> Replace ADC Interrupt and  TIA Gain and CF Settings </h3>
 *
 * @par
 *   The signal current from the photodiode is converted to a differential
 * voltage using a transimpedance amplifier (TIA). Here you can set the gain and
 * cf settings for this seperate amplifier. ( mostly for init function )
 * See hr3_replace_adc for just adc replacing.
 *
 *
 * @param seperate      -   Enable or Disable using seperate amplifier
 * @param cf_setting    -   CF setting for seperate TIA
 * @param gain_setting  -   Gain setting for seperate TIA
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_tia_gain(bool replace, uint8_t cf_setting, uint8_t gain_setting);

/**
 * @brief <h3> Replace ADC Interrupt </h3>
 *
 * @par
 *   Replaces the ADC_RDY output with a fully-programmable signal from the
 * timing engine.
 *
 *
 * @param seperate  -   true: replace adc_rdy false: do not replace adc_rdy
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_replace_adc(bool replace);

/**
 * @brief <h3> Set LED Currents </h3>
 *
 * @par
 *    The default LED current range is from 0 mA to 50 mA. The individual
 * currents of each of the three LEDs can be controlled independently, each
 * with a separate 6-bit control. Taken as a decimal number, the 6-bit setting
 * provides 63 equal steps between 0 mA and 50 mA. Each increment of the ILED
 * 6-bit code causes the LED current setting to increment by approximately 0.8 mA.
 *
 *
 * @param led1_current  -   LED 1 current (mA)
 * @param led2_current  -   LED 2 current (mA)
 * @param led3_current  -   LED 3 current (mA)
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led_currents(uint8_t led1_current, uint8_t led2_current,
                            uint8_t led3_current);

/**
 * @brief <h3> Set Dynamic Settings </h3>
 *
 * @par
 *   Function for init function to wrap all of the dynamic settings ( all
 * one register ) into a struct and cone function.
 *
 *
 * @param modes  -   dynamic_modes_t struct with all dynamic settings
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_dynamic_settings(dynamic_modes_t *modes);

/**
 * @brief <h3> Set Transmit Enable </h3>
 *
 * @par
 *   Sets the transmitter to enabled or disabled
 *
 * @param transmit  -   enum representing enable or disabled transmitter
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_transmit_enable(transmitter_t transmit);

/**
 * @brief <h3> Set Current Range </h3>
 *
 * @par
 *   Sets the LED current range multiplier
 *
 * @param curr_range  -   enum representing either normal or 2x current multiplier
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_current_range(led_current_range_t curr_range);

/**
 * @brief <h3> Set ADC Power </h3>
 *
 * @par
 *   Sets the ADC to powered down or powered on.
 *
 * @param adc_power  -   enum representing enable or disabled adc
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_adc_power(adc_pwer_t adc_power);

/**
 * @brief <h3> Set Clock Mode </h3>
 *
 * @par
 *   Sets the clock to external or internal oscillator modes.
 *
 * @param clk_mode  -   enum representing clock modes.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_clock_mode(clk_mode_t clk_mode);

/**
 * @brief <h3> Set TIA Power </h3>
 *
 * @par
 *   Sets the transimpedance amplifier (TIA) to powered down or powered on.
 *
 * @param tia_power  -   enum representing TIA powered off or powered on.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_tia_power(tia_pwer_t tia_power);

/**
 * @brief <h3> Set Rest of ADC </h3>
 *
 * @par
 *   Sets the rest of the ADC to powered on or powered off.
 *
 * @param rest_of_adc  -   enum representing rest of adc powered on / off.
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_rest_of_adc(rest_of_adc_t rest_of_adc);

/**
 * @brief <h3> Set AFE Receive </h3>
 *
 * @par
 *   Sets the AFE to normal mode (RX enabled) or RX disabled.
 *
 * @param afe_rx_mode  -   enum representing AFE Receive disabled or enabled.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_afe_receive(afe_rx_mode_t afe_rx_mode);

/**
 * @brief <h3> Set AFE Mode </h3>
 *
 * @par
 *   Sets the AFE to normal mode or powered down.
 *
 * @param afe_mode  -   enum representing AFE powered down, or normal mode.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_afe_mode(afe_mode_t afe_mode);

/**
 * @brief <h3> Set Clockout and Divider </h3>
 *
 * @par
 *  Sets the CLK pin to enabled and the frequency of the clock output on the
 * CLK pin (in internal clock mode).
 *
 * @param enable    -   Enables or Disables the usage of the CLK pin.
 * @param div       -   Frequency of the clock output on the CLK pin.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_clkout_enable(bool enable, uint8_t div);

/**
 * @brief <h3> Get LED1 Value </h3>
 *
 * @par
 *  Gets the LED1 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval LED1 Value
 *
 */
uint32_t hr3_get_led1_val(void);

/**
 * @brief <h3> Get LED2 Value </h3>
 *
 * @par
 *  Gets the LED2 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval LED2 Value
 *
 */
uint32_t hr3_get_led2_val(void);

/**
 * @brief <h3> Get LED3 Value </h3>
 *
 * @par
 *  Gets the LED3 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval LED3 Value
 *
 */
uint32_t hr3_get_led3_val(void);

/**
 * @brief <h3> Get AMB1 Value </h3>
 *
 * @par
 *  Gets the AMB1 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval AMB1 Value
 *
 */
uint32_t hr3_get_amb1_val(void);

/**
 * @brief <h3> Get LED2 Ambient 2 Value </h3>
 *
 * @par
 *  Gets the LED2 Ambient 2 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval LED2 Ambient 2 Value
 *
 */
uint32_t hr3_get_led2_amb2_val(void);

/**
 * @brief <h3> Get LED1 Ambient 1 Value </h3>
 *
 * @par
 *  Gets the LED1 Ambient 1 Value from the 24 bit register in twos complement form.
 *
 *
 * @retval LED1 Ambient 1 Value
 *
 */
uint32_t hr3_get_led1_amb1_val(void);

/**
 * @brief <h3> Is PD Shorted </h3>
 *
 * @par
 *  Checks to see if the Photodiode is shorted
 *
 * @param enable    -   Enables or Disables the usage of the CLK pin.
 * @param div       -   Frequency of the clock output on the CLK pin.
 *
 * @retval 0 = Diagnostics detected no short across the PD
 *         1 = Diagnostics detected a short across the PD
 *
 */
uint8_t hr3_is_pd_shorted(void);

/**
 * @brief <h3> Set INP and INN Settings </h3>
 *
 * @par
 *  Sets the INP and INN settings
 *
 * @param inp_inn_setting   -   enum representing INP and INN settings
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_inp_inn_settings(inm_inn_t *inp_inn_setting);

/**
 * @brief <h3> Set PDN Cycle Start and End </h3>
 *
 * @par
 *   Sets the PDN Cycle start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   PDN Cycle start ( time )
 * @param end        -   PDN Cycle end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_pdn_cycle_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set Programmable Timing Start and End </h3>
 *
 * @par
 *   Sets the Programmable Timing ( if ADC RDY is disabled )
 *  start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   Programmable Timing start ( time )
 * @param end        -   Programmable Timing end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_prgrmmbl_timing_start_end(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set LED3 Start and End </h3>
 *
 * @par
 *   Sets the LED3 start time and end time. ( See Datasheet )
 *
 *
 * @param start      -   LED sample start ( time )
 * @param end        -   LED sample end   ( time )
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_led3_start_stop(uint16_t start, uint16_t end);

/**
 * @brief <h3> Set Internal Clock Divider </h3>
 *
 * @par
 *   Sets the internal clock divider.
 *
 *
 * @param div   -   Clock division ratio for the clock to the timing engine.
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_int_clk_div(uint8_t div);

/**
 * @brief <h3> Set OFFDAC Settings </h3>
 *
 * @par
 *   Sets the OFFDAC Settings
 *
 *
 * @param settings   -   Struct representing OFFDAC settings for all LEDs / AMBs
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_offdac_settings(offdac_settings_t *settings);

/**
 * @brief <h3> Calibrate </h3>
 *
 * @par
 *   Used to take the next approximately 5 seconds to find the average
 * adc ouput.
 *
 * @retval average adc ouput
 *
 */
uint32_t hr3_calibrate(void);

/**
 * @brief <h3> Read Enable </h3>
 *
 * @par
 *   Enables Reading from registers
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_read_enable(void);

/**
 * @brief <h3> Read Disable </h3>
 *
 * @par
 *   Disables Reading from registers
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_read_disable(void);

/**
 * @brief <h3> Set Clockout Divider </h3>
 *
 * @par
 *   Sets the clockout divider for external clock
 *
 * @param enable       -       Sets clock to external mode
 * @param div          -       Divider for external clock
 *
 * @retval 0: success -1: failure
 *
 */
uint8_t hr3_set_clkout_div(bool enable, uint8_t div);

#endif /* HEARTRATE_3_HW_H */
       /*** End of File **************************************************************/