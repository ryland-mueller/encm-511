/*
 * File:   main.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-09-18
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

#define LEFT_LED_CTRL LATBbits.LATB5
#define MIDDLE_LED_CTRL LATBbits.LATB6
#define RIGHT_LED_CTRL LATBbits.LATB7

#define LEFT_BTN_STAT PORTAbits.RA4
#define RIGHT_BTN_STAT PORTBbits.RB8

int main(void) {
    
    TRISBbits.TRISB5 = 0;   // Set to output (left LED)
    TRISBbits.TRISB6 = 0;   // Set to output (middle LED)
    TRISBbits.TRISB7 = 0;   // Set to output (right LED)
    
    TRISAbits.TRISA4 = 1;   // Set to input (left button)
    TRISBbits.TRISB8 = 1;   // Set to input (right button)
    
    IOCPUAbits.IOCPA4 = 1;  // Enable pull-up (left button)
    IOCPUBbits.IOCPB8 = 1;  // Enable pull-up (right button)
    
    uint8_t state = 0;
    
    uint16_t milliseconds = 0;
    
    uint16_t i = 0;
    
    while(1){
        if(LEFT_BTN_STAT == 0 && RIGHT_BTN_STAT == 1) {
            state = 1;
        } else if(LEFT_BTN_STAT == 1 && RIGHT_BTN_STAT == 0) {
            state = 2;
        } else if (LEFT_BTN_STAT == 0 && RIGHT_BTN_STAT == 0) {
            state = 3;
        } else {
            state = 0;
        }
        
        if(state == 1 && (milliseconds % 500) > 250) {
            LEFT_LED_CTRL = 1;
            MIDDLE_LED_CTRL = 0;
            RIGHT_LED_CTRL = 0;
        } else if (state == 2 && milliseconds > 2000) {
            LEFT_LED_CTRL = 0;
            MIDDLE_LED_CTRL = 1;
            RIGHT_LED_CTRL = 0;
        } else if (state == 3) {
            LEFT_LED_CTRL = 0;
            MIDDLE_LED_CTRL = 0;
            RIGHT_LED_CTRL = 1;
        } else {
            LEFT_LED_CTRL = 0;
            MIDDLE_LED_CTRL = 0;
            RIGHT_LED_CTRL = 0;
        }
        
        i = 0;
        while (i < 444) {   // approx 1ms delay
            Nop();
            i++;
        }
        
        milliseconds++;
        
        if (milliseconds >= 4000) {  // Only ever need up to 4000ms period. Avoids overflow
            milliseconds = 0;
        }
        
    }
    
    return 0;
}
