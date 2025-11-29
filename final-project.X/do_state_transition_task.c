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

        
        // if (pb_stat == PB0_CLICKED){
        //     next_state = set_timer;
        // }
        // pb_stat = 0;
        // current_state = next_state;

        // if (current_state == set_timer){
        //     LED2 ^= 1;}

        // xSemaphoreGive(state_sem);


        switch (current_state)
        {
            case waiting_state:
                // Actions for waiting_state
                T2CONbits.TON = 0;
                // Transition logic
                if (pb_stat == PB0_CLICKED){
                    next_state = set_timer;
                }
                break;

            case set_timer:
                // Actions for set_timer
                // Transition logic
                if (pb_stat == PB1_PB2_CLICKED){
                    next_state = timer_countdown;
                }
                break;

            case timer_countdown:
                // Actions for timer_countdown
                T2CONbits.TON = 1;
                // Transition logic
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_paused;
                }
                break;

            case timer_paused:
                // Actions for timer_paused
                // Transition logic
                next_state = timer_countdown_info;
                break;

            case timer_countdown_info:
                // Actions for timer_countdown_info
                // Transition logic
                next_state = timer_info_paused;
                break;

            case timer_info_paused:
                // Actions for timer_info_paused
                // Transition logic
                next_state = timer_countdown_nblink;
                break;

            case timer_countdown_nblink:
                // Actions for timer_countdown_nblink
                // Transition logic
                next_state = timer_nblink_paused;
                break;

            case timer_nblink_paused:
                // Actions for timer_nblink_paused
                // Transition logic
                next_state = timer_countdown_info_nblink;
                break;

            case timer_countdown_info_nblink:
                // Actions for timer_countdown_info_nblink
                // Transition logic
                next_state = timer_info_nblink_paused;
                break;

            case timer_info_nblink_paused:
                // Actions for timer_info_nblink_paused
                // Transition logic
                next_state = timer_finished;
                break;

            case timer_finished:
                // Actions for timer_finished
                // Transition logic
                next_state = waiting_state;
                break;

            default:
                // Handle unexpected state
                // Transition logic
                next_state = waiting_state;
                break;
        }

    // Update current state at the end of the cycle
    pb_stat = 0;

    if(current_state != next_state)
        for (const char *p = CLEAR_SCREEN; *p != '\0'; p++) {
            xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }


    current_state = next_state;
    xSemaphoreGive(state_sem);



    }
}

