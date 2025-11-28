/*
 * File:   common.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-23
 */

#ifndef COMMON_H
#define	COMMON_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <string.h>
#include <xc.h>

#define LED0    LATBbits.LATB5
#define LED1    LATBbits.LATB6
#define LED2    LATBbits.LATB7

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9

#define SET_BIT(flags, n)       ((flags) |= (1 << (n)))
#define CHECK_BIT(flags, n)     (((flags) >> (n)) & 1)
#define CLEAR_BIT(flags, n)     ((flags) &= ~(1 << (n)))
#define TOGGLE_BIT(flags, n)    ((flags) ^= (1 << (n)))

extern uint8_t pb_stat;      // Bit-field for button status flags
#define PB0_HELD_FLAG    0   // Flag set to indicate PB0 is being held and program has not acted on that yet
#define PB0_CLICKED_FLAG 1   // Flag set to indicate PB0 has been clicked
#define PB1_HELD_FLAG    2   // Flag set to indicate PB1 is being held and program has not acted on that yet
#define PB1_CLICKED_FLAG 3   // Flag set to indicate PB1 has been clicked
#define PB2_HELD_FLAG    4   // Flag set to indicate PB2 is being held and program has not acted on that yet
#define PB2_CLICKED_FLAG 5   // Flag set to indicate PB2 has been clicked

// Define the transition combination to compare with pb_stat
#define PB0_CLICKED         (1 << PB0_CLICKED_FLAG)
#define PB1_CLICKED         (1 << PB1_CLICKED_FLAG)
#define PB2_CLICKED         (1 << PB2_CLICKED_FLAG)
#define PB0_HELD            (1 << PB0_HELD_FLAG)
#define PB1_HELD            (1 << PB1_HELD_FLAG)
#define PB2_HELD            (1 << PB2_HELD_FLAG)
#define PB0_PB1_HELD       ((1 << PB0_HELD_FLAG) | \
                            (1 << PB1_HELD_FLAG))
#define PB0_PB2_HELD       ((1 << PB0_HELD_FLAG) | \
                            (1 << PB2_HELD_FLAG))
#define PB1_PB2_HELD       ((1 << PB1_HELD_FLAG) | \
                            (1 << PB2_HELD_FLAG))
#define PB1_PB2_CLICKED   ((1 << PB1_CLICKED_FLAG) | \
                            (1 << PB2_CLICKED_FLAG))

typedef enum
{
    waiting_state,
    set_timer,
    timer_countdown,
    timer_paused,
    timer_countdown_info,
    timer_info_paused,
    timer_countdown_nblink,
    timer_nblink_paused,
    timer_countdown_info_nblink,
    timer_info_nblink_paused,
    timer_finished       
} states;

static const char WaitingMessage[] = "Please press PB0 to start";
static const char SetTimerMessage[] = "Please enter a time";
static const char ADCMessage[] = "ADC value: ";
static const char DutyMessage[] = "Duty ratio: ";

///////////////////////////////////////////////////////////////////////////////
///////  mutexs, semaphores, queues declerations here for global use  /////////
///////////////////////////////////////////////////////////////////////////////

extern SemaphoreHandle_t uart_tx_sem;       // mutex to transmit on the UART (might not be needed if only one task uses)
extern QueueHandle_t xUartTransmitQueue;    // queue to transmit on the UART

extern SemaphoreHandle_t uart_rx_sem;       // mutex to recieve on the UART (might not be needed if only one task uses)
extern QueueHandle_t xUartRecieveQueue;    // queue to transmit on the UART


extern uint16_t global_adc_value;           // global adc value
extern SemaphoreHandle_t adc_value_sem;     // and its mutex


//definitions must be in common.c
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////  Task prototypes and their inits   ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ADC task and its initalization
void do_adc_init(void);
void vDoAdcTask( void * pvParameters );

// UART transmit task and its initalization
void do_uart_transmit_init(void);
void vDoUartTransmitTask( void * pvParameters );

// UART transmit task and its initalization
void do_uart_recieve_init(void);
void vDoUartRecieveTask( void * pvParameters );

void vButtonTask( void * pvParameters);
void vStateManagerTask( void * pvParameters);

///////////////////////////////////////////////////////////////////////////////

#endif	/* COMMON_H */

