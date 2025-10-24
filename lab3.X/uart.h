/*
 * File:   uart.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART_H
#define	UART_H

void InitUART2(void);
void Disp2String(char *str);
void XmitUART2(char CharNum, unsigned int repeatNo);
void RecvUart(char* input, uint8_t buf_size);
char RecvUartChar012(void);


#endif

