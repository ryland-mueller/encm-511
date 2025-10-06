#include "xc.h"

void TimersInit(){
    //T3CON config (LED blinking timer)
    T2CONbits.T32 = 0;      // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 3;    // set prescaler to 1:256
    T3CONbits.TCS = 0;      // use internal clock (Fosc/2 = 4MHz)
    T3CONbits.TSIDL = 0;    // operate in idle mode
    IPC2bits.T3IP = 2;      // 7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;      // set timer flag to 0
    IEC0bits.T3IE = 1;      // enable timer interrupt
    PR3 = 7812;             // set the count value for 0.5 s (or 500 ms)
    TMR3 = 0;               // set timer count to 0

    //T2CON config (Debounce timer)
    T2CONbits.TCKPS = 3;    // set prescaler to 1:256
    T2CONbits.TCS   = 0;    // use internal clock (Fosc/2 = 4MHz)
    T2CONbits.TSIDL = 0;    // operate in idle mode
    IPC1bits.T2IP = 3;      // 7 is highest and 1 is lowest pri.
    IFS0bits.T2IF = 0;      // set timer flag to 0
    IEC0bits.T2IE = 1;      // enable timer interrupt
    PR2 = 782;              // set the count value for or 50 ms (to be used for debounce)
    TMR2 = 0;               // set timer count to 0

    
}

void delay_ms(uint16_t ms){
    
    uint16_t temp = IEC0; 
    IEC0 = 0;           //disable interrupts
    
    if(ms > 4095)       //avoid overflow, max delay is 4095ms
        ms = 4095;
    
    T3CONbits.TON = 0;  // stop the timer
    TMR3 = 0;           // set timer count to 0
    
    PR3 = ms * 16;     // set the count value (16 ~= 1/64e-3)
    T3CONbits.TON = 1; // start the timer
    
    IEC0 = temp;       //restore interrupts
}