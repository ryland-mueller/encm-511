/*
 * File:   main.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
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
#pragma config BSLIM = 8191     //Boot Segment Flash Page Address Limit bits->8191

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
#include "common.h"
#include "uart.h"
#include "init_functions.h"

//DEBUG
#include <stdio.h>


#define HELD_TIME 1000  // time in milliseconds a button must be pressed for it to be considered "held"
#define DEBOUNCE_TIME 40

uint8_t pb_stat = 0;

uint8_t pb_manager_flags;   // Bit-field of flags to be used by button manager logic
#define PB_UPDATE   0   // Indicates an IOC interrupt has occured
#define PB0_LAST    1   // Indicates the previous state of PB0
#define PB1_LAST    2   // Indicates the previous state of PB1
#define PB2_LAST    3   // Indicates the previous state of PB2
#define PB0_ON      4   // Indicates PB0 is currently pressed and its counter is running
#define PB1_ON      5   // Indicates PB1 is currently pressed and its counter is running
#define PB2_ON      6   // Indicates PB2 is currently pressed and its counter is running

uint16_t pb0_time = 0;   // Count of milliseconds that PB0 has been held for
uint16_t pb1_time = 0;   // Count of milliseconds that PB1 has been held for
uint16_t pb2_time = 0;   // Count of milliseconds that PB2 has been held for


#define LED0    LATBbits.LATB5
#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9


//DEBUG
char str_buffer[10];

int main(void) {
    
    IO_init();
    timer_init();
    InitUART2();
    
    while(1) {
        Idle();
        
        if(CHECK_BIT(pb_manager_flags, PB_UPDATE))
        {
            CLEAR_BIT(pb_manager_flags, PB_UPDATE);
            
            //*str_buffer = "";
            //sprintf(str_buffer, "%d", pb0_time);
            //Disp2String("pb0_time: ");
            //Disp2String(str_buffer);
            //XmitUART2('\n',1);
            //XmitUART2('\r',1);
            
            if(CHECK_BIT(pb_manager_flags, PB0_LAST) && !PB0) // Transition to pressed
                SET_BIT(pb_manager_flags, PB0_ON);  // Start timing the press
                
            if(!PB0 && pb0_time >= HELD_TIME) {     // Pressed for long enough to be "held"
                SET_BIT(pb_stat, PB0_HELD_FLAG);
                
                //DEBUG
                Disp2String("PB0 Held");
                XmitUART2('\n',1);
                XmitUART2('\r',1);
            }
            else
                CLEAR_BIT(pb_stat, PB0_HELD_FLAG);
            
            if(!CHECK_BIT(pb_manager_flags, PB0_LAST) && PB0){   // Transition to released
                
                if(pb0_time >= DEBOUNCE_TIME && pb0_time < HELD_TIME) {
                    SET_BIT(pb_stat, PB0_CLICKED_FLAG); // flag a click occurred
                
                    //DEBUG
                    Disp2String("PB0 Clicked");
                    XmitUART2('\n',1);
                    XmitUART2('\r',1);
                }
                    
                // stop timing the press
                CLEAR_BIT(pb_manager_flags, PB0_ON);
                pb0_time = 0;
            }
            
            // Update "last" flags
            if(PB0)
                SET_BIT(pb_manager_flags, PB0_LAST);
            else
                CLEAR_BIT(pb_manager_flags, PB0_LAST);
            if(PB1)
                SET_BIT(pb_manager_flags, PB1_LAST);
            else
                CLEAR_BIT(pb_manager_flags, PB1_LAST);
            if(PB2)
                SET_BIT(pb_manager_flags, PB2_LAST);
            else
                CLEAR_BIT(pb_manager_flags, PB2_LAST);
        }

        
    }
    
    return 0;
}

// Timer 1 (LED0) ISR
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    LED0 ^= 1; // toggle LED0
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

// Timer 2 (button manager) ISR
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    SET_BIT(pb_manager_flags, PB_UPDATE);   // flag that the button press logic needs to be run
    if(CHECK_BIT(pb_manager_flags, PB0_ON))
        pb0_time++;
    if(CHECK_BIT(pb_manager_flags, PB1_ON))
        pb1_time++;
    if(CHECK_BIT(pb_manager_flags, PB2_ON))
        pb2_time++;
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

// Timer 3 ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    // not used
}

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    SET_BIT(pb_manager_flags, PB_UPDATE);   // flag that the button press logic needs to be run
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}