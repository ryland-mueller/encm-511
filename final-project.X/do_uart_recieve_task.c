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

//uint8_t RXFlag = 0;

void do_uart_recieve_init(void) 
{
    xUartRecieveQueue = xQueueCreate(64, sizeof(uint8_t));

    #ifndef UART2_CONFIG
    #define UART2_CONFIG
    RPINR19bits.U2RXR = 11;     // Assign U2RX to RP11 (pin 22)
    RPOR5bits.RP10R = 5;        // Assign RP10 (pin 21) to U2TX

    U2MODEbits.BRGH = 1;        // High baud rate is enabled
    U2MODEbits.WAKE = 1;        // UART is awake and will trigger interrupt when Rx detected
    
    U2BRG = 8;                  // Baud rate = 115200
    
	U2STAbits.UTXISEL0 = 0;     // Interrupt when a character is transferred to the Transmit Shift Register 
    U2STAbits.UTXISEL1 = 0;     // (this implies there is at least one character open in the transmit buffer)
    U2STAbits.URXEN = 1;        // Receive is enabled, U2RX pin is controlled by UART2
    U2STAbits.UTXEN = 1;        // Transmit is enabled, U2TX pin is controlled by UART2
    U2STAbits.URXISEL = 0b00;   // Interrupt is set when any character is received and transferred from the RSR to the receive buffer

	IFS1bits.U2TXIF = 0;	    // Set flag to 0
    IPC7bits.U2TXIP = 3;        // Set priority
    IEC1bits.U2TXIE = 1;        // Enable interrupt

	IFS1bits.U2RXIF = 0;        // Set flag to 0
	IPC7bits.U2RXIP = 4;        // Set priority
    IEC1bits.U2RXIE = 1;        // Enable interrupt

	U2MODEbits.UARTEN = 1;	    // Enable UART Rx

	U2STAbits.UTXEN = 1;        // Enable UART Tx
	return;
    #endif
}

void vDoUartRecieveTask( void * pvParameters )
{
    for( ;; )
    {
       // block until the UART rx ISR notifies something is in the buffer
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // take uart rx mutex
        xSemaphoreTake(uart_rx_sem, portMAX_DELAY);

        // char to store char from rx buffer
        uint8_t char_received;
        
        // get char from rx buffer
        char_received = RecvUartChar();

        // give uart mutex here so it doesn't keep it while waiting for queue space
        xSemaphoreGive(uart_rx_sem);                    

        // if there was something in the buffer put it in the queue
        // xQueueSendToBack(xUartRecieveQueue, (void*)&char_recieved, portMAX_DELAY);
        xQueueSendToBack(xUartTransmitQueue, &char_received, portMAX_DELAY);
    }
}

