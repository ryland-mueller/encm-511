#include "xc.h"

void IOinit(){
    
    //Set push buttons as input and connect internal PU
    TRISAbits.TRISA4  = 1;
    IOCPUAbits.IOCPA4 = 1;  
    
    TRISBbits.TRISB8  = 1;
    IOCPUBbits.IOCPB8 = 1;
    
    TRISBbits.TRISB9  = 1;
    IOCPUBbits.IOCPB9 = 1;
    
    //Set LEDs as outputs
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB6 = 0; 
    TRISBbits.TRISB7 = 0;
    
}

void IOCinit(){
    PADCONbits.IOCON = 1;   // enable Interupt-on-Change

    //Set high and low going IOC on all three push buttons
    IOCNAbits.IOCNA4 = 1;
    IOCPAbits.IOCPA4 = 1;
    IOCNBbits.IOCNB8 = 1;
    IOCPBbits.IOCPB8 = 1;
    IOCNBbits.IOCNB9 = 1;
    IOCPBbits.IOCPB9 = 1;

    //Set the interrupt flag to 0 for the A and B registers
    IOCSTATbits.IOCPAF = 0;
    IOCSTATbits.IOCPBF = 0;
    
    IFS1bits.IOCIF = 0; // set IOC interrupt flag to 0
    IPC4bits.IOCIP = 3; // set IOC interrupt priority
    IEC1bits.IOCIE = 1; // enable global IOC interrupts
}
