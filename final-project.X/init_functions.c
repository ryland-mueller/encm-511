/*
 * File:   init_functions.c
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-21
 */

#include "string.h"
#include "common.h"
#include "init_functions.h"

// Same for all our ISRs to prevent them from pre-empting each other
#define ISR_PRIORITY 2

void IO_init(void)
{
    ANSELA = 0x0000; /* keep this line as it sets I/O pins that can also be analog to be digital */
    ANSELB = 0x0008; /* keep this line as it sets I/O pins that can also be analog to be digital */

    return;
}