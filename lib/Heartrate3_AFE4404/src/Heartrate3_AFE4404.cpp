/**
 * @file Heartrate3_AFE4404.cpp
 * @author Christoph Honal
 * @brief Implements the functions defined in Heartrate3_AFE4404.h
 * @version 0.1
 * @date 2020-08-30
 */

#include "Heartrate3_AFE4404.h"

#include "heartrate_3.h"
#include "heartrate_3_hal.h"

static volatile int cnt = 0;

Heartrate3_AFE4404::Heartrate3_AFE4404(PinName sda, PinName scl, PinName intr, events::EventQueue& event_queue):
    _i2c(sda, scl),
    _int_adc(intr),
    _event_queue(event_queue)
{
    // _i2c.frequency(50000);
}

void Heartrate3_AFE4404::init(uint8_t address)
{
    hr3_hal_init(&_i2c, address);

    hr3_set_settings(sw_reset_en, diag_mode_dis, susp_count_dis, reg_read_dis);
    hr3_set_led2_start_end( 0, 398 );
    hr3_set_led2_sample_start_end( 100, 398 );
    hr3_set_adc_reset0_start_end( 5600, 5606 );
    hr3_set_led2_convert_start_end( 5608, 6067 );
    hr3_set_led3_start_stop( 400, 798 );
    hr3_set_led3_sample_start_end( 500, 798 );
    hr3_set_adc_reset1_start_end( 6069, 6075 );
    hr3_set_led3_convert_start_end( 6077, 6536 );
    hr3_set_led1_start_end( 800, 1198 );
    hr3_set_led1_sample_start_end( 900, 1198 );
    hr3_set_adc_reset2_start_end( 6538, 6544 );
    hr3_set_led1_convert_start_end( 6546, 7006 );
    hr3_set_amb1_sample_start_end( 1300, 1598 );
    hr3_set_adc_reset3_start_end( 7008, 7014 );
    hr3_set_amb1_convert_start_end( 7016, 7475 );
    hr3_set_prgrmmbl_timing_start_end( 7477, 7577);           
    hr3_set_pdn_cycle_start_end( 7675, 39199 );
    hr3_set_prpct_count( 39999 );

    hr3_set_timer_and_average_num( true, 3 );
    hr3_set_seperate_tia_gain( true, 0, 4 );
    hr3_set_tia_gain( true, 0, 3 );
    hr3_set_led_currents( 15, 0, 0 );

    dynamic_modes_t dynamic_modes;
    dynamic_modes.transmit = trans_en;                       //Transmitter disabled
    dynamic_modes.curr_range = led_norm;                     //LED range 0 - 100
    dynamic_modes.adc_power = adc_on;                        //ADC on
    dynamic_modes.clk_mode = osc_mode;                       //Use internal Oscillator
    dynamic_modes.tia_power = tia_off;                       //TIA off
    dynamic_modes.rest_of_adc = rest_of_adc_off;             //Rest of ADC off
    dynamic_modes.afe_rx_mode = afe_rx_normal;               //Normal Receiving on AFE
    dynamic_modes.afe_mode = afe_normal;                     //Normal AFE functionality

    hr3_set_dynamic_settings( &dynamic_modes );
    hr3_set_clkout_div( false, 2 );
    hr3_set_int_clk_div( 1 );

    initStatHRM(); // Initializes values to 0

    _int_adc.rise(callback(this, &Heartrate3_AFE4404::_handleADCInterrupt)); // Setup interrupt handler
}

void Heartrate3_AFE4404::_handleADCInterrupt()
{
    cnt++;
    _event_queue.call(this, &Heartrate3_AFE4404::_handleADC);
}

void Heartrate3_AFE4404::_handleADC()
{
    // Give led1 ambient value to heartrate function. ( 100 times a second )
    statHRMAlgo(hr3_get_led1_val());
}

uint8_t Heartrate3_AFE4404::getHeartrate()
{
    return hr3_get_heartrate();
}