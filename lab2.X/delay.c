#include "delay.h"

void delay_init(void)
{
    // Timer 1 (for delay function)
    T1CONbits.TCKPS = 2;            // set prescaler to 1:64
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 0;              // disable interrupt
    PR1 = 0xFFFF;                   // max timer period (use as counter)  
}

void delay_ms(uint16_t ms)
{
    if(ms>1057)                 // avoid overflow
        ms = 1057
    TMR1 = 0;                   // reset timer
    T1CONbits.TON = 1;          // start timer
    uint16_t period = ms * (uint16_t)62;
    while(TMR1 < period) {
        Nop();                  // wait until delay has elapsed
    }
    T1CONbits.TON = 0;          // stop timer
}

