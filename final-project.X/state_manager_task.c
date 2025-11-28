#include "common.h"
#include "state_manager_task.h"

void vStateManagerTask( void * pvParameters )
{
    TickType_t LastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        xTaskDelayUntil(LastWakeTime, 50);
        
        xSemaphoreTake(pb_sem, portMAX_DELAY);
        xSemaphoreTake(state_sem, portMAX_DELAY);
        
        if(pb_change)
        {
            if(current_state == waiting_state)
            {
                if(pb0_click == 1)
                    next_state = set_timer;
                else
                    next_state = waiting_state;
            }
            if(current_state == set_timer)
            {
                if(pb0_click == 1 && pb1_click == 1)
                    next_state = timer_countdown;
                else
                    next_state = set_timer;
            }
            if(current_state == timer_countdown)
            {
                if(pb2_click == 1)
                    next_state == timer_paused;
                else if (pb2_held == 1)
                    next_state == set_timer;
                else
                    next_state == timer_countdown;
            }
            if(current_state == timer_paused)
            {
                if(pb2_click == 1)
                    next_state == timer_countdown;
                else
                    next_state == timer_paused;
            }
            if(current_state == timer_countdown_info)
            {
                if(pb2_click == 1)
                    next_state == timer_info_paused;
                else if (pb2_held == 1)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_info;
            }
            if(current_state == timer_info_paused)
            {
                if(pb2_click == 1)
                    next_state == timer_countdown_info;
                else
                    next_state == timer_info_paused;
            }
            if(current_state == timer_countdown_nblink)
            {
                if(pb2_click == 1)
                    next_state == timer_nblink_paused;
                else if (pb2_held == 1)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_nblink;
            }
            if(current_state == timer_nblink_paused)
            {
                if(pb2_click == 1)
                    next_state == timer_countdown_nblink;
                else
                    next_state == timer_nblink_paused;
            }
            if(current_state == timer_countdown_info_nblink)
            {
                if(pb2_click == 1)
                    next_state == timer_info_nblink_paused;
                else if (pb2_held == 1)
                    next_state == set_timer;
                else
                    next_state == timer_countdown_info_nblink;
            }
            if(current_state == timer_info_nblink_paused)
            {
                if(pb2_click == 1)
                    next_state == timer_countdown_info_nblink;
                else
                    next_state == timer_info_nblink_paused;
            }
        
            pb_change = 0;
            pb0_click = 0;
            pb1_click = 0;
            pb2_click = 0;
        }
        
        
        if(next_state != current_state )
        {
            if(current_state == waiting_state) 
            {
                StringToQueue(WaitingMessage);
            }
            else if(current_state == set_timer) 
            {
                StringToQueue(SetTimerMessage);
                //Display UART with the user inputs 
            }
        }
        
        xSemaphoreGive(pb_sem);
        xSemaphoreGive(state_sem);
        
    }
}
