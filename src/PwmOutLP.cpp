/**
 * @file ScreenModel.cpp
 * @author Christoph Honal
 * @brief Implements some functions defined in DisplayService.h
 * @version 0.1
 * @date 2020-08-24
 */

#include "DisplayService.h"

void PwmOutLP::enable()
{
    core_util_critical_section_enter();
    lock_deep_sleep();
    NRF_PWM0->ENABLE = 1;
    core_util_critical_section_exit();
}

void PwmOutLP::disable()
{
    core_util_critical_section_enter();
    NRF_PWM0->ENABLE = 0;
    unlock_deep_sleep();
    core_util_critical_section_exit();
}