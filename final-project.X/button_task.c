#include "common.h"
#include "button_task.h"

uint8_t pb_manager_flags = 0;   // extern in header, initialized to zero here
TickType_t pb0_time = 0;  // extern in header, initialized to zero here
TickType_t pb1_time = 0;  // extern in header, initialized to zero here
TickType_t pb2_time = 0;  // extern in header, initialized to zero here

#define HELD_TIME 1000  // time in milliseconds a button must be pressed for it to be considered "held"
#define DEBOUNCE_TIME 100

void vButtonTask( void * pvParameters )
{
    TickType_t LastWakeTime;
    TickType_t pb0_press_start = 0;
    TickType_t pb1_press_start = 0;
    TickType_t pb2_press_start = 0;

    LastWakeTime = xTaskGetTickCount(); // get current time.
    
    for(;;)
    {
        
        vTaskDelay(DEBOUNCE_TIME);          //Debounce delay
        
        // If PB0 becomes pressed, grab the tick on which the button was pressed
        if(CHECK_BIT(pb_manager_flags, PB0_LAST) && !PB0)
            pb0_press_start = xTaskGetTickCount();

        // Once PB0 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb0_press_start != 0 && !PB0)
        {
            pb0_time = (xTaskGetTickCount() - pb0_press_start);  // Calculate the on time of PB0
            if (pb0_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB0_HELD_FLAG))
            {
                SET_BIT(pb_stat, PB0_HELD_FLAG);
                pb0_time = 0;
            }
        }
        
        // If PB0 is released:
        if(!CHECK_BIT(pb_manager_flags, PB0_LAST) && PB0){
            // If PB0 was pressed only briefly, set "clicked" flag
            if(pb0_press_start != 0 && pb0_time < HELD_TIME)
                SET_BIT(pb_stat, PB0_CLICKED_FLAG);
            pb0_press_start = 0;    // Reset the PB0 press start tick count
            pb0_time = 0;        // Reset the on time of PB0
            // Make sure "held" flag is cleared since button is no longer being pressed
            CLEAR_BIT(pb_stat, PB0_HELD_FLAG);
        }

        // If PB1 becomes pressed, grab the tick count of when it was pressed
        if(CHECK_BIT(pb_manager_flags, PB1_LAST) && !PB1)
            pb1_press_start = xTaskGetTickCount();

        // Once PB1 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb1_press_start != 0 &&  !PB1)
        {
            pb1_time = xTaskGetTickCount() - pb1_press_start;
            if(pb1_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB1_HELD_FLAG)) {
                SET_BIT(pb_stat, PB1_HELD_FLAG);
                pb1_time = 0;
            }
        }
        // If PB1 is released:
        if(!CHECK_BIT(pb_manager_flags, PB1_LAST) && PB1){
            // If PB1 was pressed only briefly, set "clicked" flag
            if(pb1_press_start != 0 && pb1_time < HELD_TIME)
                SET_BIT(pb_stat, PB1_CLICKED_FLAG);
            // Reset the pb1 start time
            pb1_press_start = 0;
            pb1_time = 0;
            // Make sure "held" flag is cleared since button is no longer being pressed
            CLEAR_BIT(pb_stat, PB1_HELD_FLAG);
        }


        // If PB2 becomes pressed, start timing the press
        if(CHECK_BIT(pb_manager_flags, PB2_LAST) && !PB2)
            pb2_press_start = xTaskGetTickCount();

        // Once PB2 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb2_press_start != 0 && !PB2)
            pb2_time = xTaskGetTickCount() - pb2_press_start;
            if(pb2_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB2_HELD_FLAG)) {
                SET_BIT(pb_stat, PB2_HELD_FLAG);
                pb2_time = 0;
            }

        // If PB2 is released:
        if(!CHECK_BIT(pb_manager_flags, PB2_LAST) && PB2){
            // If PB2 was pressed only briefly, set "clicked" flag
            if(pb2_press_start != 0 && pb2_time < HELD_TIME)
                SET_BIT(pb_stat, PB2_CLICKED_FLAG);
            // Reset the pb2 start time
            pb2_press_start = 0;
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

        // do the state transitions that are done when 
        // a button is pressed
        
        //Need mutex???
        
        if(pb_stat)
        {
            if(current_state == waiting_state)
            {
                if(pb_stat == PB0_CLICKED)
                    next_state = set_timer;
                else
                    next_state = waiting_state;
            }
            if(current_state == set_timer)
            {
                if(pb_stat == PB1_PB2_CLICKED)
                    next_state = timer_countdown;
                else
                    next_state = set_timer;
            }
            if(current_state == timer_countdown)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_paused;
                else if (pb_stat == PB2_HELD)
                    next_state == set_timer;
                else
                    next_state == timer_countdown;
            }
            if(current_state == timer_paused)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_countdown;
                else
                    next_state == timer_paused;
            }
            if(current_state == timer_countdown_info)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_info_paused;
                else if (pb_stat == PB2_HELD)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_info;
            }
            if(current_state == timer_info_paused)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_countdown_info;
                else
                    next_state == timer_info_paused;
            }
            if(current_state == timer_countdown_nblink)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_nblink_paused;
                else if (pb_stat == PB2_HELD)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_nblink;
            }
            if(current_state == timer_nblink_paused)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_countdown_nblink;
                else
                    next_state == timer_nblink_paused;
            }
            if(current_state == timer_countdown_info_nblink)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_info_nblink_paused;
                else if (pb_stat == PB2_HELD)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_info_nblink;
            }
            if(current_state == timer_info_nblink_paused)
            {
                if(pb_stat == PB2_CLICKED)
                    next_state == timer_countdown_info_nblink;
                else
                    next_state == timer_info_nblink_paused;
            }
        }
    }
}
