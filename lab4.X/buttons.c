#include "common.h"
#include "buttons.h"
#include "delay.h"

// DEBUG
#include <stdio.h>

uint8_t pb_manager_flags = 0;   // extern in header, initialized to zero here
uint16_t pb0_time = 0;  // extern in header, initialized to zero here
uint16_t pb1_time = 0;  // extern in header, initialized to zero here
uint16_t pb2_time = 0;  // extern in header, initialized to zero here

#define HELD_TIME 1000  // time in milliseconds a button must be pressed for it to be considered "held"
#define DEBOUNCE_TIME 100

//DEBUG
char str_buffer[10];

void buttons_update(void)
{
    delay_ms(DEBOUNCE_TIME);
            
    // If PB0 becomes pressed, start timing the press
    if(CHECK_BIT(pb_manager_flags, PB0_LAST) && !PB0)
        SET_BIT(pb_manager_flags, PB0_ON);

    // Once PB0 has been pressed for long enough, set "held" flag if not already set, and stop timing press
    if(!PB0 && pb0_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB0_HELD_FLAG)) {
        SET_BIT(pb_stat, PB0_HELD_FLAG);
        CLEAR_BIT(pb_manager_flags, PB0_ON);
        pb0_time = 0;
    }

    // If PB0 is released:
    if(!CHECK_BIT(pb_manager_flags, PB0_LAST) && PB0){
        // If PB0 was pressed only briefly, set "clicked" flag
        if(CHECK_BIT(pb_manager_flags, PB0_ON) && pb0_time < HELD_TIME)
            SET_BIT(pb_stat, PB0_CLICKED_FLAG);
        // Stop timing the press
        CLEAR_BIT(pb_manager_flags, PB0_ON);
        pb0_time = 0;
        // Make sure "held" flag is cleared since button is no longer being pressed
        CLEAR_BIT(pb_stat, PB0_HELD_FLAG);
    }


    // If PB1 becomes pressed, start timing the press
    if(CHECK_BIT(pb_manager_flags, PB1_LAST) && !PB1)
        SET_BIT(pb_manager_flags, PB1_ON);

    // Once PB1 has been pressed for long enough, set "held" flag if not already set, and stop timing press
    if(!PB1 && pb1_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB1_HELD_FLAG)) {
        SET_BIT(pb_stat, PB1_HELD_FLAG);
        CLEAR_BIT(pb_manager_flags, PB1_ON);
        pb1_time = 0;
    }

    // If PB1 is released:
    if(!CHECK_BIT(pb_manager_flags, PB1_LAST) && PB1){
        // If PB1 was pressed only briefly, set "clicked" flag
        if(CHECK_BIT(pb_manager_flags, PB1_ON) && pb1_time < HELD_TIME)
            SET_BIT(pb_stat, PB1_CLICKED_FLAG);
        // Stop timing the press
        CLEAR_BIT(pb_manager_flags, PB1_ON);
        pb1_time = 0;
        // Make sure "held" flag is cleared since button is no longer being pressed
        CLEAR_BIT(pb_stat, PB1_HELD_FLAG);
    }


    // If PB2 becomes pressed, start timing the press
    if(CHECK_BIT(pb_manager_flags, PB2_LAST) && !PB2)
        SET_BIT(pb_manager_flags, PB2_ON);

    // Once PB2 has been pressed for long enough, set "held" flag if not already set, and stop timing press
    if(!PB2 && pb2_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB2_HELD_FLAG)) {
        SET_BIT(pb_stat, PB2_HELD_FLAG);
        CLEAR_BIT(pb_manager_flags, PB2_ON);
        pb2_time = 0;
    }

    // If PB2 is released:
    if(!CHECK_BIT(pb_manager_flags, PB2_LAST) && PB2){
        // If PB2 was pressed only briefly, set "clicked" flag
        if(CHECK_BIT(pb_manager_flags, PB2_ON) && pb2_time < HELD_TIME)
            SET_BIT(pb_stat, PB2_CLICKED_FLAG);
        // Stop timing the press
        CLEAR_BIT(pb_manager_flags, PB2_ON);
        pb2_time = 0;
        // Make sure "held" flag is cleared since button is no longer being pressed
        CLEAR_BIT(pb_stat, PB2_HELD_FLAG);
    }

    // Update "last" flags
    if(PB0)
        SET_BIT(pb_manager_flags, PB0_LAST);
    else
        CLEAR_BIT(pb_manager_flags, PB0_LAST);
    if(PB1)
        SET_BIT(pb_manager_flags, PB1_LAST);
    else
        CLEAR_BIT(pb_manager_flags, PB1_LAST);
    if(PB2)
        SET_BIT(pb_manager_flags, PB2_LAST);
    else
        CLEAR_BIT(pb_manager_flags, PB2_LAST);
}

void buttons_reset(void)
{
    pb0_time = 0;
    pb1_time = 0;
    pb2_time = 0;
}