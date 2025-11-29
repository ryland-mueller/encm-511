#include "common.h"


void vDoLED01Task(void *pvParameters)
{
    
    TickType_t LastWakeTime;        
    LastWakeTime = xTaskGetTickCount(); //get current tick
    TickType_t frequency = 250;         //frequency of the task
    
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, frequency);
        
        if (current_state == timer_finished)
        {
            LED1 ^= 1;
            LED0 = !LED1;
        }
        else
        {
            LED0 = 0;
        }
    }
}
