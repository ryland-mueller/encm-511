/*
 * File:   init_functions.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

#include "string.h"
#include "common.h"
#include "init_functions.h"

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

void IO_init(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0008; /* keep this line as it sets I/O pins that can also be analog to be digital */

    TRISBbits.TRISB3 = 1;   // Set to input (ADC_input)

    TRISAbits.TRISA4 = 1;   // Set to input (PB0)
    TRISBbits.TRISB8 = 1;   // Set to input (PB1)
    TRISBbits.TRISB9 = 1;   // Set to input (PB2)
    
    IOCPUAbits.IOCPA4 = 1;  // Enable pull-up (PB0)
    IOCPUBbits.IOCPB8 = 1;  // Enable pull-up (PB1)
    IOCPUBbits.IOCPB9 = 1;  // Enable pull-up (PB2)

    PADCONbits.IOCON = 1;   // Enable interrupt-on-change (IOC)
    
    IOCNAbits.IOCNA4 = 1;   // Enable high-to-low IOC (PB0)
    IOCPAbits.IOCPA4 = 1;   // Enable low-to-high IOC (PB0)
    IOCNBbits.IOCNB8 = 1;   // Enable high-to-low IOC (PB1)
    IOCPBbits.IOCPB8 = 1;   // Enable low-to-high IOC (PB1)
    IOCNBbits.IOCNB9 = 1;   // Enable high-to-low IOC (PB2)
    IOCPBbits.IOCPB9 = 1;   // Enable low-to-high IOC (PB2)
    
    IFS1bits.IOCIF = 0;             // Clear system-wide IOC flag
    IPC4bits.IOCIP = ISR_PRIORITY;  // Set IOC priority)
    IEC1bits.IOCIE = 1;             // Enable IOC
    return;
}


void timer_init(void)
{ 
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    // Timer 3
    T3CONbits.TCKPS = 2;            // set prescaler to 1:64
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 0;              // disable interrupt
    PR3 = 0xFFFF;                   // max timer period (use as counter)
    
    return;
}

void PWM_init(void)
{
    RPOR1bits.RP2R = 13;            // set pin 6 to PWM out pin (OC1)
    OC1R  = 0x0FFF;                 // set PWM frequency to 977Hz
    OC1RS = OC1R/2;                 // 50% duty cycle 
    OC1CON2bits.SYNCSEL = 0x1F;         // select OC1 as synchronization source
    OC1CON1bits.OCTSEL = 0;             // select timer 2 as clock source
    OC1CON1bits.OCM = 111;              // center-aligned PWM mode
}