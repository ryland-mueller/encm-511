#include "common.h"

uint8_t pb_manager_flags = 0;   // extern in header, initialized to zero here
uint16_t pb0_time = 0;  // extern in header, initialized to zero here
uint16_t pb1_time = 0;  // extern in header, initialized to zero here
uint16_t pb2_time = 0;  // extern in header, initialized to zero here

#define HELD_TIME 1000  // time in milliseconds a button must be pressed for it to be considered "held"
#define DEBOUNCE_TIME 100

void vButtonTask( void * pvParameters )
{
    TickType_t LastWakeTime;
    TickType_t PB0_press_start = 0;
    TickType_t PB1_press_start = 0;
    TickType_t PB2_press_start = 0;
    TickType_t PB0_on_time = 0;

    LastWakeTime = xTaskGetTickCount(); // get current time.
    
    for(;;)
    {
        
        vTaskDelay(DEBOUNCE_TIME);          //Debounce delay
        
        // If PB0 becomes pressed, grab the tick on which the button was pressed
        if(CHECK_BIT(pb_manager_flags, PB0_LAST) && !PB0)
            PB0_press_start = xTaskGetTickCount();

        // Once PB0 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (PB0_press_start != 0 && !PB0)
        {
            PB0_on_time = (xTaskGetTickCount() - PB0_press_start);  // Calculate the on time of PB0
            if (PB0_on_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB0_HELD_FLAG))
                SET_BIT(pb_stat, PB0_HELD_FLAG);
        }
        
        // If PB0 is released:
        if(!CHECK_BIT(pb_manager_flags, PB0_LAST) && PB0){
            // If PB0 was pressed only briefly, set "clicked" flag
            if(PB0_press_start != 0 && PB0_on_time < HELD_TIME)
                SET_BIT(pb_stat, PB0_CLICKED_FLAG);
            PB0_press_start = 0;    // Reset the PB0 press start tick count
            PB0_on_time = 0;        // Reset the on time of PB0
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

        // State Transitions based on the button presses
    }
}
