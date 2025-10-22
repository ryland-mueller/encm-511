/*
 * File:   main.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
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
#include "uart.h"


uint16_t slow = 0;
char received;

#define SET_BIT(flags, n)       ((flags) |= (1 << (n)))
#define CHECK_BIT(flags, n)     (((flags) >> (n)) & 1)
#define CLEAR_BIT(flags, n)     ((flags) &= ~(1 << (n)))
#define TOGGLE_BIT(flags, n)    ((flags) ^= (1 << (n)))

uint8_t pb_stat;
#define PB0_HELD    0   // Flag set to indicate PB0 is currently being held (after being held for >1s)
#define PB0_CLICKED 1   // Flag set to indicate PB0 has been clicked. Consumer should clear flag once acting on it
#define PB1_HELD    2   // Flag set to indicate PB1 is currently being held (after being held for >1s)
#define PB1_CLICKED 3   // Flag set to indicate PB1 has been clicked. Consumer should clear flag once acting on it
#define PB2_HELD    4   // Flag set to indicate PB2 is currently being held (after being held for >1s)
#define PB2_CLICKED 5   // Flag set to indicate PB2 has been clicked. Consumer should clear flag once acting on it

uint8_t pb_last;
#define PB0_LAST    0   // Indicates the previous state of PB0
#define PB1_LAST    1   // Indicates the previous state of PB1
#define PB2_LAST    2   // Indicates the previous state of PB2

typedef enum
{
    fast_mode_idle,
    fast_mode_PB0,
    fast_mode_PB1,
    fast_mode_PB2,
    fast_mode_PB0_PB1,
    fast_mode_PB0_PB2,
    fast_mode_PB1_PB2,
    prog_mode_idle,
    prog_mode_PB0,
    prog_mode_PB1,
    prog_mode_PB2
} states;

#define LED0    LATBbits.LATB5

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

#define DEBOUNCE_TIME 40    // in milliseconds

states currentstate = fast_mode_idle;
states prevstate = fast_mode_idle;

uint8_t blink_setting;         // Would setting this as a char be helpful

void IO_init(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */

    TRISBbits.TRISB5 = 0;   // Set to output (LED0)

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

//add timer 1 as well
void timer_init(void)
{ 
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    // Timer 1
    T1CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC2bits.T1IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T1IF = 0;              // clear interrupt flag
    IEC0bits.T1IE = 1;              // enable interrupt
    PR1 = 62496;                    // set period for 4 s

    // Timer 2
    T2CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC1bits.T2IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 1;              // enable interrupt
    PR2 = 3906;                     // set period for 0.25 s
    
    // Timer 3
    T3CONbits.TCKPS = 3;            // set prescaler to 1:256
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 1;              // enable interrupt
    PR3 = 62496;                    // set period for 4 s
}

void get_blinkrate()
{
    if (blink_setting == 0)
        PR1 = 3906;             // 0.25
    else if (blink_setting == 1)
        PR1 = 7812;             // 0.5
    else
        PR1 = 15624;            // 1s
}

int main(void) {
    
    IO_init();
    timer_init();
    InitUART2();
    
    while(1) {
        idle();

        if(pb_stat)
        {
            if(prevstate == fast_mode_idle)
            {
                if(CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB0;
                else if(!CHECK_BIT(pb_stat,PB0_CLICKED) && CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB1;
                else if(!CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB2;
                else if(!CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && CHECK_BIT(pb_stat,PB0_HELD) && CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB0_PB1;
                else if(!CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB0_PB2;
                else if(!CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && CHECK_BIT(pb_stat,PB1_HELD) && CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_PB1_PB2;
                else if(CHECK_BIT(pb_stat,PB0_CLICKED) && CHECK_BIT(pb_stat,PB1_CLICKED) && CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = prog_mode_idle;
                else
                    currentstate = fast_mode_idle;
            }
            else if (prevstate == prog_mode_idle)
            {
                if(CHECK_BIT(pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = prog_mode_PB0;
                else if(CHECK_BIT(!pb_stat,PB0_CLICKED) && CHECK_BIT(pb_stat,PB1_CLICKED) && !CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = prog_mode_PB1;
                else if(CHECK_BIT(!pb_stat,PB0_CLICKED) && !CHECK_BIT(pb_stat,PB1_CLICKED) && CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = prog_mode_PB2;
                else if(CHECK_BIT(pb_stat,PB0_CLICKED) && CHECK_BIT(pb_stat,PB1_CLICKED) && CHECK_BIT(pb_stat,PB2_CLICKED) && !CHECK_BIT(pb_stat,PB0_HELD) && !CHECK_BIT(pb_stat,PB1_HELD) && !CHECK_BIT(pb_stat,PB2_HELD))
                    currentstate = fast_mode_idle;
                    else
                    currentstate = prog_mode_idle
            }

            switch(currentstate)
            {
                case fast_mode_PB0:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB0 was pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    PR1 = 3906;              // 0.25s blinkrate
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case fast_mode_PB1:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB1 was pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    PR1 = 7812;              // 0.50s blinkrate
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case fast_mode_PB2:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB2 was pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    PR1 = 15625;              // 1s blinkrate
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case fast_mode_PB0_PB1:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB0 and PB1 are pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    LED0 = 1;
                    break;
                case fast_mode_PB0_PB2:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB0 and PB2 are pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    LED0 = 1;
                    break;
                case fast_mode_PB1_PB2:
                    pb_stat = 0;
                    Disp2String("Fast Mode: PB1 and PB2 are pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    LED0 = 1;
                    break;
                case prog_mode_PB0:
                    pb_stat = 0;
                    Disp2String("Prog Mode: PB0 was pressed");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    PR1 = 46875;              // 3s blinkrate
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case prog_mode_PB1:
                    pb_stat = 0;
                    Disp2String("Prog mode: PB1 was pressed, Setting = ");
                    XmitUART2(blink_setting,1);
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    get_blinkrate()              // Sets PR1 dependent on blinkrate setting
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case prog_mode_PB2:
                    pb_stat = 0;
                    Disp2String("Prog Mode: Blink setting = ");    // For this i think you just dont even have the x and then whatever you input is X or just put the previous input for X maybe
                    blink_setting = RecvUartChar();                // Need to rework this function still. It should display what it received as it goes though
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    PR1 = 1953;              // 0.125s blinkrate
                    if (TMR1 > PR1) 
                        TMR1 = 0;
                    break;
                case fast_mode_idle:
                    pb_stat = 0;
                    Disp2String("Fast Mode: IDLE");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    LED0 = 0;
                    break;
                case prog_mode_idle:
                    pb_stat = 0;
                    Disp2String("Prog Mode: IDLE");
                    XmitUART2('\r',1);
                    XmitUART2('\n',1);
                    LED0 = 0;
                    break;
            }

            prevstate = currentstate;
        }


        Disp2String("Enter a character (hit enter to receive): ");
        received = RecvUartChar();
        Disp2String("\r\n you entered ");
        XmitUART2(received,1);
        XmitUART2('\r',1);
        XmitUART2('\n',1);
        
    }
    
    return 0;
}

// Timer 1 (LED0) ISR
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    LED0 ^= 1; // toggle LED0
    LED0_last ^= 1;
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

// Timer 2 (LED0) ISR
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    LED0 ^= 1; // toggle LED0
    LED0_last ^= 1;
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

// Timer 3 (LED1) ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    LED1 ^= 1; // toggle LED1
    LED1_last ^= 1;
    IFS0bits.T3IF = 0; // Clear Timer 3 interrupt flag
}

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    pb_event = 1;   // flag that state machine needs to be updated
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}