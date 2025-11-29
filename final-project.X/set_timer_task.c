#include "common.h"



//Displays the user input, after the timer is started it will convert to minutes and sceonds
void vSetTimerTask( void * pvParameters )
{
    
    TickType_t LastWakeTime;
    char SetTimerBuffer[10];
    uint8_t char_received;
    uint8_t digit;
    uint32_t input_digits = 0;
    TickType_t frequency = 100;
    states temp_state = waiting_state; // state variable so mutex can be quickly released 

    LastWakeTime = xTaskGetTickCount();
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, frequency);
       
        xSemaphoreTake(state_sem, portMAX_DELAY);               // take state mutex
        temp_state = current_state;
        xSemaphoreGive(state_sem);
        
        if (current_state == set_timer)
        {

            xSemaphoreTake(countdown_sem, portMAX_DELAY);         // Take countdown mutex
            xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
            // send the cursor to the message line
            for (const char *p = MESSAGE_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Display the set timer message
            for (const char *p = SET_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }

            while (xQueueReceive(xUartReceiveQueue, &char_received, 0) == pdTRUE) {
                if (char_received >= 48 && char_received <= 57) //Is the char between 0 and 9
                {
                    digit = char_received - '0';
                    input_digits = 10 * input_digits + digit;
                    input_digits %= 10000;
                }
            }

            SetTimerBuffer[0] = ((input_digits / 1000) % 10) + '0';
            SetTimerBuffer[1] = ((input_digits / 100)  % 10) + '0';
            SetTimerBuffer[2] = ':';
            SetTimerBuffer[3] = ((input_digits / 10)   % 10) + '0';
            SetTimerBuffer[4] = ( input_digits         % 10) + '0';
            SetTimerBuffer[5] = '\0';

        
            // send the cursor to the timer line
            for (const char *p = TIMER_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            // print the current time
            for (const char *p = SetTimerBuffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            
            // If pb is pressed convert input_digits to countdown_seconds
        
            xSemaphoreGive(countdown_sem);
            xSemaphoreGive(uart_tx_queue_sem);
        }    
    }
}