#include "common.h"
#include "state_manager_task.h"

void vStateManagerTask( void * pvParameters )
{
    TickType_t LastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        xTaskDelayUntil(LastWakeTime, 50);
        //get adc values
        //get uart message
        //do state transistions based on uart message 
        
        
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
        
    }
}
