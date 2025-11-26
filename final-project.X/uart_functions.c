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


uint8_t received_char = 0;
uint8_t RXFlag = 0;

void Disp2String(char *str) //Displays String of characters
{
    unsigned int i;
    for (i=0; i<= strlen(str); i++)
    {
        XmitUART2(str[i],1);
    }

    return;
}

// Displays 16 bit number in Hex form using UART2
void Disp2Hex(unsigned int DispData){
char i;
char nib = 0x00;
XmitUART2(' ',1); // Disp Gap
XmitUART2('0',1); // Disp Hex notation 0x
XmitUART2('x',1);
for (i=3; i>=0; i--) {
nib = ((DispData >> (4*i)) & 0x000F);
if (nib >= 0x0A) {
nib = nib +0x37; //For Hex values A-F
}
else {
nib = nib+0x30; //For hex values 0-9
}
XmitUART2(nib,1);
}
XmitUART2(' ',1);
DispData = 0x0000; // Clear DispData
return;
}

// Displays 16 bit unsigned in in decimal form
void Disp2Dec(uint16_t Dec_num) {
uint8_t rem; //remainder in div by 10
uint16_t quot;
uint8_t ctr = 0; //counter
XmitUART2(' ',1); // Disp Gap
while(ctr<5) {
quot = Dec_num/(pow(10,(4-ctr)));
rem = quot%10;
XmitUART2(rem + 0x30 , 1);
ctr = ctr + 1;
}
XmitUART2(' ',1); // Disp Gap
return;
}

void XmitUART2(char CharNum, unsigned int repeatNo)
{	
    
	U2STAbits.UTXEN = 1;
	while(repeatNo!=0) 
	{
		while(U2STAbits.UTXBF==1)   // Wait for transmit buffer to not be full
		{
		}	
		U2TXREG=CharNum;            // Fill transmit register with character
		repeatNo--;
	}
	while(U2STAbits.TRMT==0)        // Wait for transmission to be completed
	{
	}

    U2STAbits.UTXEN = 0;
}

/************************************************************************
 * Receive a buf_size number of characters over UART
 * Description: This function allows you to receive buf_size number of characters from UART,
 * provided the UART module has been initialized correctly. The function currently returns
 * if the "enter" key (ASCII 0x0D) is received. The function does not handle receiving
 * the DELETE or BACKSPACE keys meaningfully. 
 * 
 * Note: there is commented-out skeleton code that could be (re)implemented to allow the function
 * return early without the ENTER key given an interrupt-set global flag. 
 ************************************************************************/
void RecvUart(char* input, uint8_t buf_size)
{	
    uint16_t i = 0;
    char last_char;
    // wait for enter key
    while (last_char != 0x0D) {
        if (RXFlag == 1) {
            // only store alphanumeric characters
            if (received_char >= 32 && received_char <= 126) {
                if (i > buf_size-2) {
                    Disp2String("\ntoo long\n\r");
                    RXFlag = 0;
                    return;
                }
                input[i] = received_char;
                i++;
                XmitUART2(received_char,1); // loop back display
                U2STAbits.OERR = 0;
            }
            last_char = received_char;
            RXFlag = 0;
        }
        // wait for next character
        
        // if (CNflag == 1) { // this allows breaking out of the busy wait if CN interrupts are enabled...
        //     add logic here
        // }
    }
}

char RecvUartChar(void)
{	
    if (RXFlag == 1) {
        U2STAbits.OERR = 0;
        RXFlag = 0;
        return received_char;
    }
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) {

	IFS1bits.U2RXIF = 0;
    
    received_char = U2RXREG;
    
    RXFlag = 1;
    
    //_LATB5 ^= 1;
}

void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
	IFS1bits.U2TXIF = 0;

}
