/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "string.h"

#include "common.h"

void InitUART2(void);
void Disp2String(char *str);
void XmitUART2(char CharNum, unsigned int repeatNo);
void RecvUart(char* input, uint8_t buf_size);
char RecvUartChar012(void);


#endif

