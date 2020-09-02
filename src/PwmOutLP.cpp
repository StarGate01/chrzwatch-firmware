/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-24
 */

#include "DisplayService.h"

void PwmOutLP::setPower(bool on)
{
    core_util_critical_section_enter();
    if(on)
    {
        lock_deep_sleep();
        NRF_PWM0->ENABLE = 1;
    }
    else
    {
        NRF_PWM0->ENABLE = 0;
        unlock_deep_sleep();
    }
    core_util_critical_section_exit();
}