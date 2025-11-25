/*
 * File:   init_functions.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INIT_FUNCTIONS_H
#define	INIT_FUNCTIONS_H

// Initializes the IO, used to clean up main
void IO_init(void);
// Initializes the timers, used to clean up main
void timer_init(void);


#endif