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


#include "common.h"
#include "uart.h"
#include "init_functions.h"
#include "delay.h"
#include "buttons.h"
#include "ADC.h"


#define BAR_POSITIONS 64

#define BAR_DIVISOR (1024 / BAR_POSITIONS)

#define AVERAGING_N 11

uint8_t pb_stat = 0;    // extern in header, initialized to zero here

char bar_char = '='; // = or - or X

uint16_t prev_reading = 0;
uint16_t prev_bar_val = 0;

uint8_t bar_update = 1;
uint8_t num_update = 1;

uint16_t adc_reading = 0;
uint8_t bar_val = 0;

uint16_t samples[AVERAGING_N] = {0};


void update_bar(void)
{
    Disp2String("\033[s");  // save cursor position
    XmitUART2('\r', 1);     // reset cursor to beginning of line
    
    // build bar graph
    XmitUART2('[', 1);
    XmitUART2(bar_char, bar_val);
    XmitUART2(' ', BAR_POSITIONS - bar_val - 1);    // minus one for max of 63rd bar char (64 positions 0-indexed)
    XmitUART2(']', 1);
    
    Disp2String("\033[u");  // restore cursor position
}

void update_num(void)
{
    Disp2String("\033[s");  // save cursor position
    Disp2Hex(adc_reading);  // transmit ADC value
    Disp2String("\033[u");  // restore cursor position
}


int main(void)
{
    
    IO_init();
    timer_init();
    InitUART2();
    adc_init();
    
    Disp2String("\033[?25l");   // Hide cursor
    Disp2String("\033[2J");     // Clear screen
    Disp2String("\r");          // Return cursor
    
    XmitUART2(' ', BAR_POSITIONS + 1);    // move cursor to 'home'
    
    while(1)
    {
        // MOVING AVERAGE FILTER
        // shift old values back in averaging array
        for(int i = 0; i < AVERAGING_N - 1; i++) {
            samples[i] = samples[i + 1];
        }
        // Sample ADC to latest value in averaging array
        samples[AVERAGING_N - 1] = do_adc();
        // calculate average
        adc_reading = 0;
        for(int i = 0; i < AVERAGING_N; i++) {
            adc_reading += samples[i];
        }
        adc_reading = adc_reading / AVERAGING_N;
        
        if(adc_reading != prev_reading) {
            
            num_update = 1;
            prev_reading = adc_reading;
            
            bar_val = adc_reading / BAR_DIVISOR;
            
            if(bar_val != prev_bar_val) {
                bar_update = 1;
                prev_bar_val = bar_val;
            }
        }
        
        if(bar_update) {
            update_bar();
            bar_update = 0;
        }
        if(num_update) {
            update_num();
            num_update = 0;
        }
                
        delay_ms(10);
    }
    
    return 0;
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

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    SET_BIT(pb_manager_flags, PB_UPDATE);   // flag that the button press logic needs to be run
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}