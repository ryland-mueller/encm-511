/*
 * File:   uart.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */


#include "string.h"
#include "common.h"
#include "uart_functions.h"
#include <math.h>


//uint8_t RXFlag = 0;

void do_button_init(void) 
{
    TRISBbits.TRISB5 = 0;   // Set to output (LED0)
    TRISBbits.TRISB6 = 0;   // Set to output (LED1)
    TRISBbits.TRISB7 = 0;   // Set to output (LED2`)

    TRISBbits.TRISB3 = 1;   // Set to input (ADC_input)

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
    IPC4bits.IOCIP = ISR_PRIORITY;  // Set IOC priority
    IEC1bits.IOCIE = 1;             // Enable IOC
	return;
}

void vDoButtonTask(void *pvParameters)
{
    TickType_t LastWakeTime;
    const TickType_t SamplePeriod = 10;    // sample every 10 ticks

    LastWakeTime = xTaskGetTickCount();

    uint8_t stable_pb0_status = PB0;       // debounced state
    uint8_t last_stable_status = stable_pb0_status;
    uint8_t raw_pb0_status;
    uint8_t debounceCounter = 0;
    const uint8_t debounceThreshold = 5;   // require ~50ms stable

    LED0 = 0; // start OFF

    for (;;)
    {
        vTaskDelayUntil(&LastWakeTime, SamplePeriod);

        raw_pb0_status = PB0; // read raw input

        if (raw_pb0_status != stable_pb0_status) {
            // input changed, start counting
            debounceCounter++;
            if (debounceCounter >= debounceThreshold) {
                // accept new stable state
                stable_pb0_status = raw_pb0_status;
                debounceCounter = 0;

                // act only on transitions of debounced state
                if (stable_pb0_status == 0 && last_stable_status == 1) {
                    // button pressed (active low)
                    LED0 = 0;
                }
                else if (stable_pb0_status == 1 && last_stable_status == 0) {
                    // button released
                    LED0 = 1;
                }

                last_stable_status = stable_pb0_status;
            }
        } else {
            // reset counter if input matches stable state
            debounceCounter = 0;
        }
    }
}

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    
    //LED2 ^= 1;
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}