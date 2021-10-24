/****************************************************************************
* Title                 :   Heart Rate 3 Application Layer
* Filename              :   heartrate_3.c
* Author                :   CAL
* Origin Date           :   05/11/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date     Software Version     Initials      Description
*  05/11/2016    XXXXXXXXXXX         CAL      Interface Created.
*
*****************************************************************************/
/** @file XXX.c
 *  @brief This module contains the 
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "heartrate_3.h"
#include "heartrate_3_hw.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

//TI HRM Library Variables start
unsigned long peakWindowHP[21], lastOnsetValueLED1, lastPeakValueLED1;
unsigned char HR[12], HeartRate, temp;
unsigned int lastPeak, lastOnset;
unsigned long movingWindowHP;
unsigned char ispeak = 0;
unsigned char movingWindowCount, movingWindowSize, smallest, foundPeak, totalFoundPeak;
unsigned int frequency;
unsigned long currentRatio = 0;
//TI HRM Library Variables end

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

void hr3_get_values(led_values_t& values)
{
    values.led1_val = hr3_get_led1_val();
    values.led2_val = hr3_get_led2_val();
    values.led3_val = hr3_get_led3_val();
}

uint8_t hr3_get_heartrate(void)
{
    uint8_t i = 0;
    uint32_t temp = 0;

    for (i = 0; i < 12; i++)
        temp += HR[i];
    temp /= 12;

    return temp;
}

//TI HRM Library Functions
void initStatHRM(void)
{
    unsigned char i;

    // Init HR variables
    lastPeak = 0;
    lastOnset = 0;
    movingWindowHP = 0;
    movingWindowCount = 0;

    for (i = 20; i >= 1; i--)
        peakWindowHP[(unsigned char)(i - 1)] = 0;

    for (i = 12; i >= 1; i--)
        HR[(unsigned char)(i - 1)] = 0;

    // Sampling frequency
    frequency = 100;
    // Moving average window size (removes high frequency noise)
    movingWindowSize = frequency / 50;
    // Length of the shortest pulse possible
    smallest = frequency * 60 / 220;
    foundPeak = 0;
    totalFoundPeak = 0;
    HeartRate = 0;
}

void statHRMAlgo(unsigned long ppgData)
{
    unsigned char i;
    // moving average calculation
    movingWindowHP += ppgData;

    if (movingWindowCount > movingWindowSize)
    {
        // Data processing
        movingWindowCount = 0;
        // update data buffer
        updateWindow(peakWindowHP, movingWindowHP, movingWindowSize + 1);
        // reset moving average
        movingWindowHP = 0;
        ispeak = 0;
        if (lastPeak > smallest)
        {
            // looking for a local maximum using the 20 point buffer
            ispeak = 1;
            for (i = 10; i >= 1; i--)
            {
                if (peakWindowHP[10] < peakWindowHP[(unsigned int)(10 - i)])
                    ispeak = 0;
                if (peakWindowHP[10] < peakWindowHP[(unsigned int)(10 + i)])
                    ispeak = 0;
            }
            if (ispeak == 1)
            {
                // if we have a local maximum
                // values for SPO2 ratio
                lastPeakValueLED1 = findMax(peakWindowHP);
                totalFoundPeak++;

                if (totalFoundPeak > 2)
                {
                    // Update the HR and SPO2 buffer
                    updateHeartRate(HR, frequency, lastPeak);
                }
                ispeak = 1;
                lastPeak = 0;
                foundPeak++;
            }
        }

        if ((lastOnset > smallest) && (ispeak == 0))
        {
            // looking for a local minimum using the 20 point buffer
            ispeak = 1;
            for (i = 10; i >= 1; i--)
            {
                if (peakWindowHP[10] > peakWindowHP[(unsigned int)(10 - i)])
                    ispeak = 0;
                if (peakWindowHP[10] > peakWindowHP[(unsigned int)(10 + i)])
                    ispeak = 0;
            }

            // if we have a local minimum
            if (ispeak == 1)
            {
                // values for SPO2 ratio
                lastOnsetValueLED1 = findMin(peakWindowHP);
                totalFoundPeak++;
                if (totalFoundPeak > 2)
                {
                    // Update the HR and SPO2 buffer
                    //currentRatio= updateSPO2(SPO2,lastOnsetValueIR,lastOnsetValueRed,lastPeakValueIR,lastPeakValueRed);

                    // If you wanted to run an auto calibration here is the ratio that should be used
                    // AutoCalibrate=peakRed/ onsetRed;
                    // AutoCalibrate ratio should be greater that 1-2% if not you need to increase the LED current or adjust the setttings
                }
                lastOnset = 0;
                foundPeak++;
            }
        }

        if (foundPeak > 2)
        {
            // Every 4 new peaks update return values
            foundPeak = 0;
            temp = chooseRate(HR);
            if ((temp > 40) && (temp < 220))
                HeartRate = temp;
        }
    }
    movingWindowCount++;
    lastOnset++;
    lastPeak++;
}

void updateWindow(unsigned long *peakWindow, unsigned long Y, unsigned char n)
{
    // Moving average buffer for LED data
    unsigned char i;

    for (i = 20; i >= 1; i--)
    {
        peakWindow[i] = peakWindow[(unsigned char)(i - 1)];
    }
    peakWindow[0] = (Y / n);
}

unsigned char chooseRate(unsigned char *rate)
{
    // Returns the average rate, after removing the lowest and highest values (based on the number of found HR removing 2-4-6 values).
    unsigned char max, min, i, nb;
    unsigned int sum, fullsum;
    max = rate[0];
    min = rate[0];
    sum = 0;
    nb = 0;
    for (i = 7; i >= 1; i--)
    {
        if (rate[(unsigned int)(i - 1)] > 0)
        {
            if (rate[(unsigned int)(i - 1)] > max)
            {
                max = rate[(unsigned int)(i - 1)];
            }
            if (rate[(unsigned int)(i - 1)] < min)
            {
                min = rate[(unsigned int)(i - 1)];
            }
            sum += rate[(unsigned int)(i - 1)];
            nb++;
        }
    }

    if (nb > 2)
        fullsum = (sum - max - min) * 10 / (nb - 2);
    else
        fullsum = (sum)*10 / (nb);

    sum = fullsum / 10;

    if (fullsum - sum * 10 > 4)
        sum++;
    return sum;
}

void updateHeartRate(unsigned char *rate, unsigned int freq, unsigned int last)
{
    // Adds a new Heart rate into the array and lose the oldest
    unsigned char i;
    i = 60 * freq / last;
    if ((i > 40) && (i < 220))
    {
        for (i = 11; i >= 1; i--)
        {
            rate[i] = rate[(unsigned char)(i - 1)];
        }
        rate[0] = 60 * freq / last;
    }
}

unsigned long findMax(unsigned long *X)
{
    // Finds the maximum around the center of the buffer
    unsigned long res = X[8];
    unsigned char i;
    for (i = 12; i >= 9; i--)
    {
        if (res < X[i])
            res = X[i];
    }
    return res;
}

unsigned long findMin(unsigned long *X)
{
    // Finds the minimum around the center of the buffer
    unsigned long res = X[8];
    unsigned char i;
    for (i = 12; i >= 9; i--)
    {
        if (res > X[i])
            res = X[i];
    }
    return res;
}

//TI HRM Library functions end

/*************** END OF FUNCTIONS ***************************************************************************/