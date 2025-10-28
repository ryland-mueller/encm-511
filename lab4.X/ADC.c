/*
 * File:   ADC.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

#include "ADC.h"
#include "common.h"

void ADC_init(void)
{
    AD1CON2bits.PVCFG = 00;     //Selects Positive voltage reference
    AD1CON2bits.NVCFG0 = 0;     //Selects Negative voltage reference
    
    //Setup Channel 0 sample A
    AD1CHSbits.CH0NA = 0;       //Negative input as selected as gnd
    AD1CHSbits.CH0SA = 5;       //Positive input selected as AN5 (PIN7)
    
    AD1CON3bits.ADCS = 0;       //A/D Conversion clock is set to Tcy or 4MHz
    AD1CON1bits.SSRC = 0;       //ADC occurs based off SAMP 
    AD1CON1bits.FORM = 1;       //Data output is absolute decimal unsigned right justified
    AD1CON5bits.ASEN = 0;       //Disable auto scan
    AD1CON1bits.DMAEN = 0;      //Disable DMA 
    AD1CON2bits.SMPI = 0;       //Interrupts at the completion of each sample
    AD1CON1bits.MODE12 = 0;     //10-bit ADC mode
    
    AD1CON1bits.ADON = 1;       //Enable ADC
};