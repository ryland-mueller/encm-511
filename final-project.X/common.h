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
#include <xc.h>

#define LED0    LATBbits.LATB5
#define LED1    LATBbits.LATB6
#define LED2    LATBbits.LATB7

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9

#define MESSAGE_HOME "\033[1;1H"
#define TIMER_HOME "\033[2;1H"
#define ADC_HOME   "\033[3;1H"

#define ADC_MESSAGE "ADC VALUE: "
#define SET_MESSAGE "Please enter a time to countdown"
#define COUNTDOWN_MESSAGE "The time remaining on the counter is:"

#define ISR_PRIORITY 3

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

extern states current_state;
extern states next_state;

///////////////////////////////////////////////////////////////////////////////
///////  mutexs, semaphores, queues declerations here for global use  /////////
///////////////////////////////////////////////////////////////////////////////

extern SemaphoreHandle_t uart_tx_sem;       // mutex to transmit on the UART (might not be needed if only one task uses)
extern QueueHandle_t xUartTransmitQueue;    // queue to transmit on the UART

extern SemaphoreHandle_t uart_rx_sem;       // mutex to recieve on the UART (might not be needed if only one task uses)
extern QueueHandle_t xUartReceiveQueue;    // queue to transmit on the UART

extern uint16_t global_adc_value;           // global adc value
extern SemaphoreHandle_t adc_value_sem;     // and its mutex

extern uint16_t set_time;                   //Global variable that holds the set time by the user
extern uint16_t countdown_seconds;          //Global timer variable
extern SemaphoreHandle_t countdown_sem;     //Mutex for safely changing the countdown seconds

extern SemaphoreHandle_t uart_tx_queue_sem; //Mutex for adding to the transmit queue
extern SemaphoreHandle_t state_sem;         //Mutex for accessing the current state

//definitions must be in common.c
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////  Task prototypes and their inits   ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ADC task and its initalization
void do_adc_init(void);
void vDoAdcTask( void * pvParameters );
extern TaskHandle_t DoUartAdcTaskHandle;

// UART transmit task and its initalization
void do_uart_transmit_init(void);
void vDoUartTransmitTask( void * pvParameters );
extern TaskHandle_t DoUartTransmitTaskHandle;

// UART transmit task and its initalization
void do_uart_recieve_init(void);
void vDoUartRecieveTask( void * pvParameters );
extern TaskHandle_t DoUartRecieveTaskHandle;

// Timer task and its initalization
void do_timer_init(void);
void vDoTimerTask( void * pvParameters );
extern TaskHandle_t DoTimerTaskHandle;

// Set timer tasks
void vSetTimerTask( void * pvParameters );
extern TaskHandle_t SetTimerTaskHandle;

// Button task and initalization
void do_button_init(void);
void vDoButtonTask( void * pvParameters );
extern TaskHandle_t DoButtonTaskHandle;

// State transistion task and its initalization
void do_state_transition_init(void);
void vDoStateTransitionTask( void * pvParameters );
extern TaskHandle_t DoStateTransitionHandle;

///////////////////////////////////////////////////////////////////////////////

#endif	/* COMMON_H */

