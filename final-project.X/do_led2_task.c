#include "common.h"

//should be in set timer
uint16_t duty_ratio;


void do_timer3_init(void)
{ 
    //Timer 3 setup
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    T3CONbits.TCKPS = 1;            // set prescaler to 1:8
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 1;              // enable interrupt
    PR3 = 62500;                    // set period for 1s
      
    return;
}

void vDoLED2Task(void *pvParameters)
{
    TickType_t LastWakeTime;
    LastWakeTime = xTaskGetTickCount(); // get current time.
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, pdMS_TO_TICKS(10));
        // take the adc global value mutex
        xSemaphoreTake(adc_value_sem, portMAX_DELAY);   // take mutex
        
        //duty_ratio = global_adc_value / 1023;
        
        // determine number of pr from this
        PR3 = (global_adc_value * 5000)/ 1023;
        
        LED2 = 1;           //Turn LED2 on 
        T3CONbits.TON = 1;  // turn timer on 
        
        xSemaphoreGive(adc_value_sem);                  // give mutex
    }
}

// Timer 3 ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    
    // notify the timer task and notify the scheduler it should run
    LED2 = 0;
    // turn timer off
    T3CONbits.TON = 0;
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}