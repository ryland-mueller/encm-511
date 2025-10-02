/*
 * File:   main.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-02
 */

// FSEC
#pragma config BWRP = OFF       //Boot Segment Write-Protect bit->Boot Segment may be written
#pragma config BSS = DISABLED   //Boot Segment Code-Protect Level bits->No Protection (other than BWRP)
#pragma config BSEN = OFF       //Boot Segment Control bit->No Boot Segment
#pragma config GWRP = OFF       //General Segment Write-Protect bit->General Segment may be written
#pragma config GSS = DISABLED   //General Segment Code-Protect Level bits->No Protection (other than GWRP)
#pragma config CWRP = OFF       //Configuration Segment Write-Protect bit->Configuration Segment may be written
#pragma config CSS = DISABLED   //Configuration Segment Code-Protect Level bits->No Protection (other than CWRP)
#pragma config AIVTDIS = OFF    //Alternate Interrupt Vector Table bit->Disabled AIVT

// FBSLIM
#pragma config BSLIM = 8191    //Boot Segment Flash Page Address Limit bits->8191

// FOSCSEL
#pragma config FNOSC = FRC          //Oscillator Source Selection->Internal Fast RC (FRC)
#pragma config PLLMODE = PLL96DIV2  //PLL Mode Selection->96 MHz PLL. Oscillator input is divided by 2 (8 MHz input)
#pragma config IESO = OFF           //Two-speed Oscillator Start-up Enable bit->Start up with user-selected oscillator source

// FOSC
#pragma config POSCMD = NONE    //Primary Oscillator Mode Select bits->Primary Oscillator disabled
#pragma config OSCIOFCN = ON    //OSC2 Pin Function bit->OSC2 is general purpose digital I/O pin
#pragma config SOSCSEL = OFF    //SOSC Power Selection Configuration bits->Digital (SCLKI) mode
#pragma config PLLSS = PLL_FRC  //PLL Secondary Selection Configuration bit->PLL is fed by the on-chip Fast RC (FRC) oscillator
#pragma config IOL1WAY = ON     //Peripheral pin select configuration bit->Allow only one reconfiguration
#pragma config FCKSM = CSECMD   //Clock Switching Mode bits->Clock switching is enabled,Fail-safe Clock Monitor is disabled

// FWDT
#pragma config WDTPS = PS32768      //Watchdog Timer Postscaler bits->1:32768
#pragma config FWPSA = PR128        //Watchdog Timer Prescaler bit->1:128
#pragma config FWDTEN = ON_SWDTEN   //Watchdog Timer Enable bits->WDT Enabled/Disabled (controlled using SWDTEN bit)
#pragma config WINDIS = OFF         //Watchdog Timer Window Enable bit->Watchdog Timer in Non-Window mode
#pragma config WDTWIN = WIN25       //Watchdog Timer Window Select bits->WDT Window is 25% of WDT period
#pragma config WDTCMX = WDTCLK      //WDT MUX Source Select bits->WDT clock source is determined by the WDTCLK Configuration bits
#pragma config WDTCLK = LPRC        //WDT Clock Source Select bits->WDT uses LPRC

// FPOR
#pragma config BOREN = ON       //Brown Out Enable bit->Brown Out Enable Bit
#pragma config LPCFG = OFF      //Low power regulator control->No Retention Sleep
#pragma config DNVPEN = ENABLE  //Downside Voltage Protection Enable bit->Downside protection enabled using ZPBOR when BOR is inactive

// FICD
#pragma config ICS = PGD1       //ICD Communication Channel Select bits->Communicate on PGEC1 and PGED1
#pragma config JTAGEN = OFF     //JTAG Enable bit->JTAG is disabled

// FDEVOPT1
#pragma config ALTCMPI = DISABLE    //Alternate Comparator Input Enable bit->C1INC, C2INC, and C3INC are on their standard pin locations
#pragma config TMPRPIN = OFF        //Tamper Pin Enable bit->TMPRN pin function is disabled
#pragma config SOSCHP = ON          //SOSC High Power Enable bit (valid only when SOSCSEL = 1->Enable SOSC high power mode (default)
#pragma config ALTI2C1 = ALTI2CEN   //Alternate I2C pin Location->SDA1 and SCL1 on RB9 and RB8


#include "xc.h"


#define LED0    LATBbits.LATB5
#define LED1    LATBbits.LATB6
#define LED2    LATBbits.LATB7

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9


uint16_t PB3_event;
uint16_t toggle = 0;


void IOinit(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    TRISBbits.TRISB5 = 0;   // Set to output (LED0)
    TRISBbits.TRISB6 = 0;   // Set to output (LED1)
    TRISBbits.TRISB7 = 0;   // Set to output (LED2)
    
    TRISAbits.TRISA4 = 1;   // Set to input (PB0)
    TRISBbits.TRISB8 = 1;   // Set to input (PB1)
    TRISBbits.TRISB9 = 1;   // Set to input (PB2)
    
    IOCPUAbits.IOCPA4 = 1;  // Enable pull-up (PB0)
    IOCPUBbits.IOCPB8 = 1;  // Enable pull-up (PB1)
    IOCPUBbits.IOCPB9 = 1;  // Enable pull-up (PB2)
    
    PADCONbits.IOCON = 1;   // Enable interrupt-on-change (IOC)
    
    //IOCNBbits.IOCNB9 = 1;   // Enable high-to-low IOC (PB2)
    IOCPBbits.IOCPB9 = 1;   // Enable low-to-high IOC (PB2)
    
    IFS1bits.IOCIF = 0;     // Clear system-wide IOC flag
    IPC4bits.IOCIP = 3;     // Set IOC priority
    IEC1bits.IOCIE = 1;     // Enable IOC
}


void delay_ms(uint16_t ms)
{
    
}


int main(void)
{
    IOinit();
    
    /*
    // T3CON config
    T2CONbits.T32 = 0;      // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 3;    // set prescaler to 1:8
    T3CONbits.TCS = 0;      // use internal clock
    T3CONbits.TSIDL = 0;    // operate in idle mode
    IPC2bits.T3IP = 2;      // 7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;      // enable timer interrupt
    PR3 = 7812;             // set the count value for 0.5 s (or 500 ms)
    TMR3 = 0;
    T3CONbits.TON = 1;
    */
    
    
    
    
    LED1 = 0;
    
    /* Let's clear some flags */
    PB3_event = 0;
    
        
    while(1) {        
        
        Idle();
                
        if (PB3_event) {
            PB3_event = 0;
            LED1 ^= 1;           
        }
    }
    
    return 0;
}


/*
// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T2IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T3IF = 0;
    LED0 ^= 1;
}
*/

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    PB3_event = 1;
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}



