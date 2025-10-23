/* 
 * File:   delay.h
 * Author: Ryland
 *
 * Created on October 7, 2025, 2:06 PM
 */

#ifndef DELAY_H
#define	DELAY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "xc.h"

// setup Timer 1 for delaying
void delay_init(void);

// delays up to ~1000ms
void delay_ms(uint16_t ms);

#ifdef	__cplusplus
}
#endif

#endif	/* DELAY_H */

