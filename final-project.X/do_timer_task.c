#include "common.h"

uint16_t time_counted;
uint16_t local_copy;

void do_timer_init(void)
{ 
    //Timer 2 setup

    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    T2CONbits.TCKPS = 2;            // set prescaler to 1:64
    IPC1bits.T2IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T2IF = 0;              // clear interrupt flag
    IEC0bits.T2IE = 1;              // enable interrupt
    PR2 = 62500;                    // set period for 1s
      
    return;
}

void vDoTimerTask(void *pvParameters)
{
    
    char buffer[10];
    uint16_t time_remaining;
    TickType_t LastWakeTime;
    TickType_t frequency = 200;
    
    for (;;) 
    {
        vTaskDelayUntil(&LastWakeTime, frequency);
        
        xSemaphoreTake(countdown_sem, portMAX_DELAY);   // take mutex
        local_copy = countdown_seconds;
        xSemaphoreGive(countdown_sem);                      // give mutex
        xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
        xSemaphoreTake(state_sem, portMAX_DELAY);     // take uart mutex
        
        if( current_state == timer_countdown
         || current_state == timer_paused
         || current_state == timer_countdown_info
         || current_state == timer_info_paused
         || current_state == timer_countdown_nblink
         || current_state == timer_nblink_paused
         || current_state == timer_countdown_info_nblink
         || current_state == timer_info_nblink_paused)
        {
            // determine the time remaining
            time_remaining = local_copy - time_counted;
            if (time_remaining == 0)
                vTaskNotifyGiveFromISR(DoStateTransitionHandle, NULL);
            
            // send the cursor to the message line
            for (const char *p = MESSAGE_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Display the countdown message
            for (const char *p = COUNTDOWN_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            
            // send the cursor to the timer line
            for (const char *p = TIMER_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }

            int minutes = time_remaining / 60;
            int seconds = time_remaining % 60;
            buffer[0] = (minutes / 10) + '0';
            buffer[1] = (minutes % 10) + '0';
            buffer[2] = ':';
            buffer[3] = (seconds / 10) + '0';
            buffer[4] = (seconds % 10) + '0';
            buffer[5] = '\0';

            // print the current time
            for (const char *p = buffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
        }
        else
            time_counted = 0;
        
        
        xSemaphoreGive(uart_tx_queue_sem);
        xSemaphoreGive(state_sem);

    }
}

// Timer 2 ISR
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    
    LED1 ^= 1;
    time_counted ++;
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

