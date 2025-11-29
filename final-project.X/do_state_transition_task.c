#include "common.h"


void do_state_transition_init(void)
{
    current_state = waiting_state;
    next_state = waiting_state;
}

void vDoStateTransitionTask( void * pvParameters )
{

    TickType_t LastWakeTime;
    const TickType_t Frequency = 100;    // Perform an action every n ticks.
    
    LastWakeTime = xTaskGetTickCount(); // get current time.

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &LastWakeTime, Frequency );
        
        // Perform action here.
        xSemaphoreTake(state_sem, portMAX_DELAY);     // take uart mutex

        if (pb_stat == PB0_CLICKED){
            next_state = set_timer;
        }
        pb_stat = 0;
        current_state = next_state;

        if (current_state == set_timer){
            LED2 ^= 1;}

        xSemaphoreGive(state_sem);
    }
}

