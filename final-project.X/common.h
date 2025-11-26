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
#include "uart.h"
#include "semphr.h"
#include <xc.h>

#define PB0     PORTAbits.RA4
#define PB1     PORTBbits.RB8
#define PB2     PORTBbits.RB9

#define SET_BIT(flags, n)       ((flags) |= (1 << (n)))
#define CHECK_BIT(flags, n)     (((flags) >> (n)) & 1)
#define CLEAR_BIT(flags, n)     ((flags) &= ~(1 << (n)))
#define TOGGLE_BIT(flags, n)    ((flags) ^= (1 << (n)))

///////////////////////////////////////////////////////////////////////////////
///////  mutexs and semaphores declerations here for global use  //////////////
///////////////////////////////////////////////////////////////////////////////

extern SemaphoreHandle_t uart_sem;          

extern uint16_t global_adc_value;          // global adc value
extern SemaphoreHandle_t adc_value_sem;    // and its mutex

//definitions must be in common.c
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////////////  Task prototypes  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void vDoAdcTask( void * pvParameters );

///////////////////////////////////////////////////////////////////////////////

#endif	/* COMMON_H */

