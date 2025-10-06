/*
 * File:   main.c
 * Author: ENTER GROUP MEMBER NAME(S) HERE
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-XX-XX
 * 
 * FAILURE TO UPDATE THIS HEADER WITH YOUR GROUP MEMBER NAMES
 * MAY RESULT IN PENALTIES
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
#include "IoFunctions_header.h"
#include "TimerFunctions_header.h"

#define LED0 LATBbits.LATB6
#define LED1 LATBbits.LATB7
#define LED2 LATBbits.LATB5

#define PB0  PORTAbits.RA4
#define PB1  PORTBbits.RB8
#define PB2  PORTBbits.RB9

uint16_t PB0_state = 1;
uint16_t PB1_state = 1;
uint16_t PB2_state = 1;

uint16_t PB2_blink_rate = 62512; // ~4sec

int main(void) {
    IOinit();
    IOCinit();
    TimersInit();
    
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    
      
    LED0 = 0;
    LED1 = 0;
    LED2 = 0;

    // Start the timer right before going to idle
    T3CONbits.TON = 1;


    while(1) {        
        
        Idle();
        
        if ((PB0_state==0) & (PB1_state==1)){
            //TMR3 = 0;               // set timer count to 0
            PR3 = 3907;             // set the count value for 250ms
            T3CONbits.TON = 1;
        }
        else if ((PB0_state==0) & (PB1_state==0)){
            //TMR3 = 0;               // set timer count to 0
            PR3 = 7814;             // set the count value for 500ms
            T3CONbits.TON = 1;
        }
        else if ((PB0_state==1) & (PB1_state==0)){
            //TMR3 = 0;               // set timer count to 0
            PR3 = PB2_blink_rate;   // set the count value for 250ms
            T3CONbits.TON = 1;
        }
        else{
           T3CONbits.TON = 0;
           LED0 = 0;
        }
    }
    
    return 0;
}


// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    
    if(IOCSTATbits.IOCPAF){  // check if there was a button update in the A register
        if (PB0 != PB0_state)
            TMR3 = 0;
        
        PB0_state = PB0;

        IOCSTATbits.IOCPAF = 0; // clear flag
    }

    if(IOCSTATbits.IOCPBF){  // check if there was a button update in the B register
        
        if((PB0==1) & (PB1==1)){         // check only if the other buttons are not pressed
            if ((PB2==0) & (PB2_state==1))          //check if PB2 has been pressed and released
                PB2_blink_rate = PB2_blink_rate/2;

            if (PB2_blink_rate < 1952)      // reset the blink count if below 0.125s
                PB2_blink_rate = 62512;
        }
        
        if (PB1 != PB1_state)
            TMR3 = 0;
        
        PB1_state = PB1;
        PB2_state = PB2;

        IOCSTATbits.IOCPBF = 0; // clear flag
    }
    
    T2CONbits.TON = 0;  // turn off timer
    TMR2 = 0;           // reset the timer
    IFS0bits.T2IF = 0;  // clear timer 2 flag
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //Don't forget to clear the timer 3 interrupt flag!
    LED0 ^= 1;
   
    IFS0bits.T3IF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    
    if(T2CONbits.TON == 0){ // check the debounce timer is not currently going
        T2CONbits.TON = 1;  // start the debounce timer
    }
    
    IFS1bits.IOCIF = 0; // clear IOC flag
}



