/*
 * File:   ADC.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

#include "ADC.h"
#include "common.h"

void adc_init(void)
{
    AD1CON2bits.PVCFG = 0;      //Selects Positive voltage reference
    AD1CON2bits.NVCFG0 = 0;     //Selects Negative voltage reference
    
    //Setup Channel 0 sample A
    AD1CHSbits.CH0NA = 0;       //Negative input is selected as gnd
    AD1CHSbits.CH0SA = 5;       //Positive input selected as AN5 (PIN7)
    
    AD1CON3bits.ADCS = 255;     //AD Conversion clock is set to 256 * Tcy 
    AD1CON1bits.SSRC = 7;       //ADC occurs based off SAMP 
    AD1CON1bits.FORM = 0;       //Data output is absolute decimal unsigned right justified
    AD1CON5bits.ASEN = 0;       //Disable auto scan
    AD1CON1bits.DMAEN = 0;      //Disable DMA 
    AD1CON2bits.SMPI = 0;       //Interrupts at the completion of each sample
    AD1CON1bits.MODE12 = 0;     //10-bit ADC mode
    AD1CON1bits.ASAM = 0;       //Sampling starts when SAMP is set manually
    
    AD1CON1bits.ADON = 1;       //Enable ADC
    AD1CON1bits.SAMP = 0; 
}


uint16_t do_adc(void)
{
    uint16_t value = 0;
    
    AD1CON1bits.SAMP = 1;       // start A/D conversion
    //delay_ms(1);
    
    while (!AD1CON1bits.DONE){} // wait for ADC read to finish
    
    AD1CON1bits.SAMP = 0;
    
    value = ADC1BUF0;     //  set value to lower 16bits of adc buffer
    
    return value;
    
    
}
