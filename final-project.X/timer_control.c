#include "common.h"

void vButtonTask( void * pvParameters )
{
    TickType_t LastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        xTaskDelayUntil(LastWakeTime, 1);
        //get adc values
        //get uart message
        //do state transistions based on uart message 
        
        if(current_state == waiting_state) 
        {
            //Display UART to say "Press PB0 to start"
        }
        else if(current_state == set_timer) 
        {
            //Display UART to say "Please enter a time"
            //Display UART with the user inputs 
        }
        else if(current_state == timer_countdown)
        {
            //Display the timer as it counts down
        }
        else if(current_state == timer_countdown_info)
        {
            //Display the timer as it counts down
            //Display raw ADC value  and duty ratio of PWM
        }
        else if(current_state == timer_countdown_info_nblink)
        {
            //Display the timer as it counts down
        }
        else if(current_state == timer_countdown_info_nblink)
        {
            //Display the timer as it counts down
            //Display raw ADC value  and duty ratio of PWM
        }
        else if (current_state == timer_paused)
        {
            //Display the timer and pause the counting
        }
        else if (current_state == timer_info_paused)
        {
            //Display the timer and pause the counting
            //Display raw ADC value  and duty ratio of PWM
        }
        else if (current_state == timer_info_nblink_paused)
        {
            //Display the timer and pause the counting
            //Display raw ADC value  and duty ratio of PWM
        }
        else if (current_state == timer_nblink_paused)
        {
            //Display the timer and pause the counting
        }
    }
}
