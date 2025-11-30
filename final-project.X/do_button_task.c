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

// button task init
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

// button task
void vDoButtonTask(void *pvParameters)
{
    TickType_t LastWakeTime;
    const TickType_t SamplePeriod = 10;    // sample every 10 ticks (~10ms if tick=1ms)

    LastWakeTime = xTaskGetTickCount();

    const uint8_t debounceThreshold = 5;   // require ~50ms stable
    const TickType_t holdThreshold = 100;  // 100 samples * 10ms = 1 second

    // debounced state 
    uint8_t stable_pb0_status = PB0;       
    uint8_t stable_pb1_status = PB1;
    uint8_t stable_pb2_status = PB2;        

    // last stable (debounced) state 
    uint8_t last_stable_status0 = stable_pb0_status;
    uint8_t last_stable_status1 = stable_pb1_status;
    uint8_t last_stable_status2 = stable_pb2_status;
   
    // raw status
    uint8_t raw_pb0_status;
    uint8_t raw_pb1_status;
    uint8_t raw_pb2_status;
    
    // debounce counter for each button
    uint8_t debounceCounter0 = 0;
    uint8_t debounceCounter1 = 0;
    uint8_t debounceCounter2 = 0;

    // hold detection for each button
    TickType_t holdCounter0 = 0;
    TickType_t holdCounter1 = 0;
    TickType_t holdCounter2 = 0;

    for (;;)
    {
        vTaskDelayUntil(&LastWakeTime, SamplePeriod);

        // read raw input of each PB
        raw_pb0_status = PB0; 
        raw_pb1_status = PB1;
        raw_pb2_status = PB2;

        xSemaphoreTake(state_sem, portMAX_DELAY);     // take uart mutex
        
    /////////////   PB0   //////////////////////
        if (raw_pb0_status != stable_pb0_status) {
            debounceCounter0++;
            if (debounceCounter0 >= debounceThreshold) {
                stable_pb0_status = raw_pb0_status;
                debounceCounter0 = 0;

                if (stable_pb0_status == 0 && last_stable_status0 == 1) {
                    // button pressed (active low)
                    holdCounter0 = 0; // start counting hold time
                }
                else if (stable_pb0_status == 1 && last_stable_status0 == 0) {
                    // button released                   
                    
                    if(CHECK_BIT(pb_stat,PB0_HELD_FLAG))
                        CLEAR_BIT(pb_stat,PB0_HELD_FLAG);

                    else
                        SET_BIT(pb_stat,PB0_CLICKED_FLAG);

                    holdCounter0 = 0;
                }

                last_stable_status0 = stable_pb0_status;
            }
        } else {
            debounceCounter0 = 0;
        }

        // check for hold while pressed
        if (stable_pb0_status == 0) { // still pressed
            holdCounter0++;
            if (holdCounter0 >= holdThreshold) {
                SET_BIT(pb_stat,PB0_HELD_FLAG);
            }
        }
    
    /////////////   PB2   //////////////////////
        if (raw_pb1_status != stable_pb1_status) {
        debounceCounter1++;
        if (debounceCounter1 >= debounceThreshold) {
            stable_pb1_status = raw_pb1_status;
            debounceCounter1 = 0;

            if (stable_pb1_status == 0 && last_stable_status0 == 1) {
                // button pressed (active low)
                holdCounter1 = 0; // start counting hold time
            }
            else if (stable_pb1_status == 1 && last_stable_status1 == 0) {
                // button released                   
                
                if(CHECK_BIT(pb_stat,PB1_HELD_FLAG))
                    CLEAR_BIT(pb_stat,PB1_HELD_FLAG);

                else
                    SET_BIT(pb_stat,PB1_CLICKED_FLAG);

                holdCounter1 = 0;
            }

            last_stable_status1 = stable_pb1_status;
        }
    } else {
        debounceCounter1 = 0;
    }

        // check for hold while pressed
        if (stable_pb1_status == 0) { // still pressed
            holdCounter1++;
            if (holdCounter1 >= holdThreshold) {
                SET_BIT(pb_stat,PB1_HELD_FLAG);
            }
        }

        /////////////   PB2   //////////////////////
            if (raw_pb2_status != stable_pb2_status) {
            debounceCounter2++;
            if (debounceCounter2 >= debounceThreshold) {
                stable_pb2_status = raw_pb2_status;
                debounceCounter2 = 0;

                if (stable_pb2_status == 0 && last_stable_status0 == 1) {
                    // button pressed (active low)
                    holdCounter2 = 0; // start counting hold time
                }
                else if (stable_pb2_status == 1 && last_stable_status2 == 0) {
                    // button released                   
                    
                    if(CHECK_BIT(pb_stat,PB2_HELD_FLAG))
                        CLEAR_BIT(pb_stat,PB2_HELD_FLAG);

                    else
                        SET_BIT(pb_stat,PB2_CLICKED_FLAG);

                    holdCounter2 = 0;
                }

                last_stable_status2 = stable_pb2_status;
            }
        } else {
            debounceCounter2 = 0;
        }

        // check for hold while pressed
        if (stable_pb2_status == 0) { // still pressed
            holdCounter2++;
            if (holdCounter2 >= holdThreshold) {
                SET_BIT(pb_stat,PB2_HELD_FLAG);
            }
        }   

        xSemaphoreGive(state_sem);     // take uart mutex
    }
}

// Interrupt-on-change ISR
void __attribute__ ((interrupt, no_auto_psv)) _IOCInterrupt(void) {
    
    //LED2 ^= 1;
    IFS1bits.IOCIF = 0; // Clear system-wide IOC flag
}