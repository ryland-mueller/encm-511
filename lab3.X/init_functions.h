/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INIT_FUNCTIONS_H
#define	INIT_FUNCTIONS_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "string.h"

#include "common.h"

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

// Initializes the IO, used to clean up main
void IO_init(void);
// Initializes the timers, used to clean up main
void timer_init(void);



#endif	/* XC_HEADER_TEMPLATE_H */