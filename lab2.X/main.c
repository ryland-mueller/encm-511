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
//#define LED2    LATBbits.LATB7

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

#define DEBOUNCE_TIME 40    // in milliseconds


// 0 = No PBs pressed, all LEDs off
// 1 = Only PB0 pressed, LED0 blinks at 0.25 sec interval
// 2 = PB0 and PB1 are pressed, LED0 blinks at 0.5 sec interval
// 3 = Only PB1 pressed, LED1 blinks at PB2_BLINK_RATE (defined in assignment)
uint8_t state = 0;

uint8_t pb_event = 0;   // Flag that leaving Idle() is for IOC

uint8_t pb2_last = 0;   // for detecting release of PB2


void IOinit(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
    TRISBbits.TRISB5 = 0;   // Set to output (LED0)
    TRISBbits.TRISB6 = 0;   // Set to output (LED1)
    //TRISBbits.TRISB7 = 0;   // Set to output (LED2)
    
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
}


void Timer_init(void)
{
    // Timer 1 (for delay function)
    T1CONbits.TCKPS = 2;            // set prescaler to 1:64
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 0;              // disable interrupt
    PR1 = 0xFFFF;                   // max timer period (use as counter)  
    
    T2CONbits.T32 = 0;  // Operate timers 2 & 3 as separate 16-bit timers
    
    // Timer 2 (LED0)
    T2CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC1bits.T2IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 1;              // enable interrupt
    PR2 = 3906;                     // set period for 0.25 s
    
    // Timer 3 (LED1)
    T3CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 1;              // enable interrupt
    PR3 = 62496;                    // set period for 4 s
}


// delays up to ~1000ms
void delay_ms(uint16_t ms)
{
    TMR1 = 0;                   // reset timer
    T1CONbits.TON = 1;          // start timer
    uint16_t period = ms * (uint16_t)62;
    while(TMR1 < period) {
        Nop();                  // wait until delay has elapsed
    }
    T1CONbits.TON = 0;          // stop timer
}


void blink_rate_update(void)
{
    if(PR3 <= 1953) {   // if blink period is at 0.125s, reset to 4s
        PR3 = 62496;
    } else {            // divide blink period by 2
        PR3 = PR3 / 2;
        // prevent overflow if timer count is past new period
        if (TMR3 > PR3) {
            TMR3 = 0;
        }
    }
}


int main(void)
{
    
    Timer_init();
    IOinit();
        
    while(1) {        
        
        Idle();
                
        if (pb_event) {
            
            delay_ms(DEBOUNCE_TIME);
            
            // PB2 transition to released and PB0/PB1 are not pressed
            if(!pb2_last && PB2 && PB0 && PB1) {
                blink_rate_update();
            }
            
            // State machine inputs
            if (PB0 == 0 && PB1 == 1) {
                state = 1;
            } else if (PB0 == 0 && PB1 == 0) {
                state = 2;
            } else if (PB0 == 1 && PB1 == 0) {
                state = 3;
            } else {
                state = 0;
            }
            
            // State machine outputs
            if (state == 0) {
                
                T2CONbits.TON = 0;  // disable LED0 timer
                LED0 = 0;           // turn off LED0
                T3CONbits.TON = 0;  // disable LED1 timer
                LED1 = 0;           // turn off LED1
                
            } else if (state == 1) {
                
                PR2 = 3906;         // set LED0 timer period for 0.25 s
                
                // prevent overflow if timer count is past new period
                if (TMR2 > PR2) {
                    TMR2 = 0;
                }
                
                T2CONbits.TON = 1;  // enable LED0 timer
                T3CONbits.TON = 0;  // disable LED1 timer
                LED1 = 0;           // turn off LED1
                
            } else if (state == 2) {
                
                PR2 = 7812;         // set LED0 timer period for 0.5 s
                T2CONbits.TON = 1;  // enable LED0 timer
                T3CONbits.TON = 0;  // disable LED1 timer
                LED1 = 0;           // turn off LED1
                
            } else if (state == 3) {
                
                T2CONbits.TON = 0;  // disable LED0 timer
                LED0 = 0;           // turn off LED0
                T3CONbits.TON = 1;  // enable LED1 timer
                
            }
            
            pb_event = 0;
            pb2_last = PB2;
        }
    }
    return 0;
}


// Timer 2 (LED0) ISR
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    LED0 ^= 1; // toggle LED0
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

// Timer 3 (LED1) ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    LED1 ^= 1; // toggle LED1
    IFS0bits.T3IF = 0; // Clear Timer 3 interrupt flag
}

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    pb_event = 1;   // flag that state machine needs to be updated
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}