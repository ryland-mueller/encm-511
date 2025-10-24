#include "xc.h"
#include "delay.h"

void delay_ms(uint16_t ms)
{
    if(ms>1057)                 // avoid overflow
        ms = 1057;
    TMR3 = 0;                   // reset timer
    T3CONbits.TON = 1;          // start timer
    uint16_t period = ms * (uint16_t)62;
    while(TMR3 < period) {
        Nop();                  // wait until delay has elapsed
    }
    T3CONbits.TON = 0;          // stop timer
}

