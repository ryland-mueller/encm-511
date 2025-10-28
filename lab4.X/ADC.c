/*
 * File:   uart.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

#include "common.h"



uint16_t do_adc(void)
{
    uint16_t value = 0;
    
    AD1CON1bits.SAMP = 1;       // start A/D conversion
    delay_ms(1);
    AD1CON1bits.SAMP = 0;
    
    while (!AD1CON1bits.DONE){} // wait for ADC read to finish
        
    value = ADC1BUF0;     //  set value to lower 16bits of adc buffer
    
    return value;
    
    
}