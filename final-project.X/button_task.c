#include "common.h"
#include "button_task.h"

TickType_t pb0_time = 0;  // extern in header, initialized to zero here
TickType_t pb1_time = 0;  // extern in header, initialized to zero here
TickType_t pb2_time = 0;  // extern in header, initialized to zero here




#define HELD_TIME 1000  // time in milliseconds a button must be pressed for it to be considered "held"
#define DEBOUNCE_TIME 50

void vButtonTask( void * pvParameters )
{
    TickType_t LastWakeTime;
    TickType_t pb0_press_start = 0;
    TickType_t pb1_press_start = 0;
    TickType_t pb2_press_start = 0;
    
    TickType_t pb0_time = 0;  // extern in header, initialized to zero here
    TickType_t pb1_time = 0;  // extern in header, initialized to zero here
    TickType_t pb2_time = 0;  // extern in header, initialized to zero here
    
    LastWakeTime = xTaskGetTickCount(); // get current time.
    
    for(;;)
    {
        
        vTaskDelayUntil(LastWakeTime,DEBOUNCE_TIME);          //Debounce delay
        
        xSemaphoreTake(pb_sem, portMAX_DELAY);      // Ensure no one is reading these buttons while being changed
        
        // If PB0 becomes pressed, grab the tick on which the button was pressed
        if(pb0_last && !PB0){
            pb0_press_start = xTaskGetTickCount();
        }
        // Once PB0 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb0_press_start != 0 && !PB0)
        {
            pb0_time = (xTaskGetTickCount() - pb0_press_start);  // Calculate the on time of PB0
            if (pb0_time >= HELD_TIME && !CHECK_BIT(pb_stat, PB0_HELD_FLAG))
            {
                pb0_held = 1;
                pb_change = 1;
            }
        }
        
        // If PB0 is released:
        if(!pb0_last && PB0){
            // If PB0 was pressed only briefly, set "clicked" flag
            if(pb0_press_start != 0 && pb0_time < HELD_TIME)
            {
                pb0_click = 1;
                pb_change = 1;
            }
            pb0_press_start = 0;    // Reset the PB0 press start tick count
            pb0_time = 0;        // Reset the on time of PB0
            // Make sure "held" flag is cleared since button is no longer being pressed
            pb0_held = 1;;
        }

        // If PB1 becomes pressed, grab the tick count of when it was pressed
        if(pb1_last && !PB1)
            pb1_press_start = xTaskGetTickCount();

        // Once PB1 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb1_press_start != 0 &&  !PB1)
        {
            pb1_time = xTaskGetTickCount() - pb1_press_start;
            if(pb1_time >= HELD_TIME && !pb1_held) {
                pb1_held = 1 ;
                pb_change = 1;
            }
        }
        // If PB1 is released:
        if(!pb1_last && PB1){
            // If PB1 was pressed only briefly, set "clicked" flag
            if(pb1_press_start != 0 && pb1_time < HELD_TIME)
            {
                pb1_click = 1;
                pb_change = 1;
            }
            // Reset the pb1 start time
            pb1_press_start = 0;
            pb1_time = 0;
            // Make sure "held" flag is cleared since button is no longer being pressed
            pb1_held = 0;
        }


        // If PB2 becomes pressed, start timing the press
        if(pb2_last && !PB2)
            pb2_press_start = xTaskGetTickCount();

        // Once PB2 has been pressed for long enough, set "held" flag if not already set, and stop timing press
        if (pb2_press_start != 0 && !PB2)
            pb2_time = xTaskGetTickCount() - pb2_press_start;
            if(pb2_time >= HELD_TIME && !pb2_held) {
                pb_change = 1;
                pb2_held = 1;
            }

        // If PB2 is released:
        if(!pb2_last && PB2){
            // If PB2 was pressed only briefly, set "clicked" flag
            if(pb2_press_start != 0 && pb2_time < HELD_TIME)
            {
                pb2_click = 1;
                pb_change = 1;
            }
            // Reset the pb2 start time
            pb2_press_start = 0;
            pb2_time = 0;
            // Make sure "held" flag is cleared since button is no longer being pressed
            pb2_held = 0;
        }

        // Update "last" flags
        pb0_last = PB0;
        pb1_last = PB1;
        pb2_last = PB2;
        
        xSemaphoreGive(pb_sem);
    }
}
