#include "init_functions.h"

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

void IO_init(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */

    TRISBbits.TRISB5 = 0;   // Set to output (LED0)
    TRISBbits.TRISB6 = 0;   // Set to output (LED1)
    
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

//add timer 1 as well
void timer_init(void)
{ 
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    // Timer 1 (Used to blink LED0)
    T1CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC0bits.T1IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T1IF = 0;              // clear interrupt flag
    IEC0bits.T1IE = 1;              // enable interrupt
    PR1 = 62496;                    // set period for 4 s

    // Timer 2 (Used to determine click vs hold)
    T2CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC1bits.T2IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 1;              // enable interrupt
    PR2 = 3906;                     // set period for 0.25 s
    
    // Timer 3 (Used for delay)
    T3CONbits.TCKPS = 2;            // set prescaler to 1:64
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 0;              // disable interrupt
    PR1 = 0xFFFF;                   // max timer period (use as counter)
    
    return;
}