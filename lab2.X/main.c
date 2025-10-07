/*
 * File:   main.c
 * Author: Bogdan Bacea, Patrick Mitneko, Ryland Mueller
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-09-30
 * 
 * FAILURE TO UPDATE THIS HEADER WITH YOUR GROUP MEMBER NAMES
 * MAY RESULT IN PENALTIES
 */

// FSEC
#pragma config BWRP = OFF    //Boot Segment Write-Protect bit->Boot Segment may be written
#pragma config BSS = DISABLED    //Boot Segment Code-Protect Level bits->No Protection (other than BWRP)
#pragma config BSEN = OFF    //Boot Segment Control bit->No Boot Segment
#pragma config GWRP = OFF    //General Segment Write-Protect bit->General Segment may be written
#pragma config GSS = DISABLED    //General Segment Code-Protect Level bits->No Protection (other than GWRP)
#pragma config CWRP = OFF    //Configuration Segment Write-Protect bit->Configuration Segment may be written
#pragma config CSS = DISABLED    //Configuration Segment Code-Protect Level bits->No Protection (other than CWRP)
#pragma config AIVTDIS = OFF    //Alternate Interrupt Vector Table bit->Disabled AIVT

// FBSLIM
#pragma config BSLIM = 8191    //Boot Segment Flash Page Address Limit bits->8191

// FOSCSEL
#pragma config FNOSC = FRC    //Oscillator Source Selection->Internal Fast RC (FRC)
#pragma config PLLMODE = PLL96DIV2    //PLL Mode Selection->96 MHz PLL. Oscillator input is divided by 2 (8 MHz input)
#pragma config IESO = OFF    //Two-speed Oscillator Start-up Enable bit->Start up with user-selected oscillator source

// FOSC
#pragma config POSCMD = NONE    //Primary Oscillator Mode Select bits->Primary Oscillator disabled
#pragma config OSCIOFCN = ON    //OSC2 Pin Function bit->OSC2 is general purpose digital I/O pin
#pragma config SOSCSEL = OFF    //SOSC Power Selection Configuration bits->Digital (SCLKI) mode
#pragma config PLLSS = PLL_FRC    //PLL Secondary Selection Configuration bit->PLL is fed by the on-chip Fast RC (FRC) oscillator
#pragma config IOL1WAY = ON    //Peripheral pin select configuration bit->Allow only one reconfiguration
#pragma config FCKSM = CSECMD    //Clock Switching Mode bits->Clock switching is enabled,Fail-safe Clock Monitor is disabled

// FWDT
#pragma config WDTPS = PS32768    //Watchdog Timer Postscaler bits->1:32768
#pragma config FWPSA = PR128    //Watchdog Timer Prescaler bit->1:128
#pragma config FWDTEN = ON_SWDTEN    //Watchdog Timer Enable bits->WDT Enabled/Disabled (controlled using SWDTEN bit)
#pragma config WINDIS = OFF    //Watchdog Timer Window Enable bit->Watchdog Timer in Non-Window mode
#pragma config WDTWIN = WIN25    //Watchdog Timer Window Select bits->WDT Window is 25% of WDT period
#pragma config WDTCMX = WDTCLK    //WDT MUX Source Select bits->WDT clock source is determined by the WDTCLK Configuration bits
#pragma config WDTCLK = LPRC    //WDT Clock Source Select bits->WDT uses LPRC

// FPOR
#pragma config BOREN = ON    //Brown Out Enable bit->Brown Out Enable Bit
#pragma config LPCFG = OFF    //Low power regulator control->No Retention Sleep
#pragma config DNVPEN = ENABLE    //Downside Voltage Protection Enable bit->Downside protection enabled using ZPBOR when BOR is inactive

// FICD
#pragma config ICS = PGD1    //ICD Communication Channel Select bits->Communicate on PGEC1 and PGED1
#pragma config JTAGEN = OFF    //JTAG Enable bit->JTAG is disabled

// FDEVOPT1
#pragma config ALTCMPI = DISABLE    //Alternate Comparator Input Enable bit->C1INC, C2INC, and C3INC are on their standard pin locations
#pragma config TMPRPIN = OFF    //Tamper Pin Enable bit->TMPRN pin function is disabled
#pragma config SOSCHP = ON    //SOSC High Power Enable bit (valid only when SOSCSEL = 1->Enable SOSC high power mode (default)
#pragma config ALTI2C1 = ALTI2CEN    //Alternate I2C pin Location->SDA1 and SCL1 on RB9 and RB8


#include "xc.h"
#include "delayms.h"

uint8_t timerdone;
uint8_t PB_event_occured;
uint16_t PB2_last = 0;
uint8_t state;
uint32_t blink_rate;

/**
 * You might find it useful to add your own #defines to improve readability here
 */

void delay_ms(uint16_t ms)
{
    //timerdone = 0;
    TMR2 = 0;
    PR2 =  ms / 0.064;    // set the count value for 0.5 s (or 500 ms) 
    T2CONbits.TON = 1;          //Turn on the timer
    while (timerdone == 0)
       Nop();
    timerdone = 0;
    T2CONbits.TON = 0;
}

void IOinit()
{
    // set pins for LED
    TRISBbits.TRISB5 = 0; //LED0
    TRISBbits.TRISB6 = 0; //LED1
    
    
    // set pin for button
    TRISAbits.TRISA4 = 1;   //Sets pin as input
    IOCPUAbits.CNPUA4 = 1;  //Enables pullup
    
    TRISBbits.TRISB8 = 1;   //Sets pin as input 
    IOCPUBbits.CNPUB8 = 1;  //Enables pull up
    
    TRISBbits.TRISB9 = 1;   //Sets pin as input 
    IOCPUBbits.CNPUB9 = 1;  //Enables pull up
}

void Timerinit()
{
    //T3CON config
    T2CONbits.T32 = 0;     // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 3;   // set prescaler to 1:256
    T3CONbits.TCS = 0;     // use internal clock
    T3CONbits.TSIDL = 0;   //operate in idle mode
    IPC2bits.T3IP = 2;     //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;     //enable timer interrupt
    PR3 = 7812;            // set the count value for 0.5 s (or 500 ms)
    TMR3 = 0;              //Resets the timer vale
    T3CONbits.TON = 0;     //Turns on the timer
    
    //T2CON config
    T2CONbits.TCKPS = 3; // set prescaler to 1:256
    T2CONbits.TCS = 0;   // use internal clock
    T2CONbits.TSIDL = 0; //operate in idle mode
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;   //enable timer interrupt
    TMR2 = 0;            //Resets timer value
    T2CONbits.TON = 0;   //Turns on the timer
}

void IOCinit()
{
    PADCONbits.IOCON = 1;   //Enables IOC
    IOCNBbits.IOCNB8 = 1;   //Sets PB1 to trigger IOC on negative edge 
    IOCPBbits.IOCPB8 = 1;   //Sets PB1 to trigger IOC on positive edge
    IOCSTATbits.IOCPBF = 0; //Clears PortB IOC status
    
    IOCNAbits.IOCNA4 = 1;   //Sets PB0 to trigger IOC on negative edge 
    IOCPAbits.IOCPA4 = 1;   //Sets PB0 to trigger IOC on positive edge
    IOCSTATbits.IOCPAF = 0; //Clears PortA IOC status
    
    IOCNBbits.IOCNB9 = 1;   //Sets PB2 to trigger IOC on negative edge 
    IOCPBbits.IOCPB9 = 1;   //Sets PB2 to trigger IOC on positive edge

    
    IFS1bits.IOCIF = 0;     //Clears interrupt request flag
    IPC4bits.IOCIP = 3;     //Sets interrupt priority
    IEC1bits.IOCIE = 1;     //enables the IOC interrupt
}

int blink_rate_update()
{
    //divide blink rate by 2
    blink_rate /= 2;
    //if blink rate is less than 0.125s, restart at 4s
    if (blink_rate <1953)
        blink_rate = 62500;
}


#define LED0    LATBbits.LATB5
#define LED1    LATBbits.LATB6

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9



int main(void) {

    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    IOinit();
    Timerinit();
    IOCinit();
    
    //Set Initial Values
    LED0 = 0;
    LED1 = 0;
    blink_rate = 62500;
    PB2_last = 1;
    
    //Clear Flags
    PB_event_occured = 0;
    state = 0;

        
    while(1) {        
        
        Idle();
        
        delay_ms(60);
         
        if (PB_event_occured)
        {    
            //add logic for if ONLY pb2 is pressed PB2_last == 0 & PB2 = 1 * both other pb = 1
            if (PB2_last == 0 && PB2 == 1 && PB1 == 1 && PB0 == 1)
                blink_rate_update();
                        
            if (PB0 == 1 && PB1 == 1)
                state = 0;                  //no PB is pressed
            else if (PB0 == 0 && PB1 == 1)
                state = 1;                  //PB0 is pressed
            else if (PB0 == 1 && PB1 == 0)
                state = 2;                  //PB1 is pressed
            else if (PB0 == 0 && PB1 == 0)
                state = 3;                  //Both PB pressed
            
            if (state == 0)           //turn off all lights   
            {
                TMR3 = 0;
                LED0 = 0;             //Turn LED0 off
                LED1 = 0;             //Turn LED1 off
                T3CONbits.TON = 0;    //Turns off the timer used to blink lights
            }
            else if (state == 1)      //LED0 blink rate is 0.25s
            {
                T3CONbits.TON = 0;
                TMR3 = 0;
                PR3 = 3906;           //Set timer to count to 0.25s for blink rate
                LED0 = 1;             //Turn LED0 on
                LED1 = 0;             //Turn LED1 off
                T3CONbits.TON = 1;    //Turns on the timer used to blink lights
            }
            else if (state == 2)      //LED1 has variable blink rate
            {
                T3CONbits.TON = 0;
                TMR3 = 0;
                PR3 = blink_rate;          //Sets timer to value depending on amount of PB2 presses
                LED0 = 0;             //Turn LED0 off
                LED1 = 1;             //Turn LED1 on
                T3CONbits.TON = 1;    //Turns on the timer used to blink lights
            }
            else if (state == 3)      //LED0 blink rate is 0.5s
            {
                T3CONbits.TON = 0;
                TMR3 = 0;
                PR3 = 7812;           //Set timer to count to 0.5s for blink rate
                LED0 = 1;             //Turn LED0 on
                LED1 = 0;             //Turn LED1 off
                T3CONbits.TON = 1;    //Turns on the timer used to blink lights
            }
            else                            //Default State
            {
                TMR3 = 0;
                LED0 = 0;             //Turn LED0 off
                LED1 = 0;             //Turn LED1 off
                T3CONbits.TON = 0;    //Turns off the timer used to blink lights

            }
        
        PB2_last = PB2;    
        PB_event_occured = 0;           //Clear the event occurred flag
        }
    }
    return 0;
}

// Timer 1 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    //Don't forget to clear the timer 1 interrupt flag!
    IFS0bits.T1IF = 0;
}

// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T2IF = 0;
    timerdone = 1;
}

//This timer is used to blink LED
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //Don't forget to clear the timer 2 interrupt flag!
    IFS0bits.T3IF = 0;
    if (state == 1 || state == 3)
        LED0 ^= 1;
    else if (state == 2)
        LED1 ^= 1;
}

void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    IFS1bits.IOCIF = 0;
    PB_event_occured = 1;
}


