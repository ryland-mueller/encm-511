#include "common.h"


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
    uint8_t current_time = 'A';

    for (;;) {
        // wait for notify from timer ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // send the cursor to the timer line
        for (const char *p = TIMER_HOME; *p != '\0'; p++) {
            xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }

        // print the current time
        xQueueSendToBack(xUartTransmitQueue, &current_time, portMAX_DELAY);
    }
}

// Timer 2 ISR
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    
    // notify the timer task and notify the scheduler it should run
    vTaskNotifyGiveFromISR(DoTimerTaskHandle, NULL);
    
    LED0 ^= 1;
    
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

