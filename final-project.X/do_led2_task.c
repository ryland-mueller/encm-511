#include "common.h"

//should be in set timer
uint32_t duty_ratio;
uint16_t pwm_period = 10;
int8_t step = 1;
static uint16_t counter = 0;

void do_timer3_init(void)
{ 
    //Timer 3 setup
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    T3CONbits.TON = 0;
    T3CONbits.TCKPS = 0;            // set prescaler to 1:1
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 1;              // enable interrupt
    //TMR3 = 0;
    PR3 = 8000;                     // set period for 1ms
    T3CONbits.TON = 1; 
    return;
}

void vDoLED2Task(void *pvParameters)
{
    TickType_t LastWakeTime;
    LastWakeTime = xTaskGetTickCount(); // get current time.
    TickType_t frequency = 100;
   
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, frequency );
        
        if (current_state == waiting_state)
        {
            duty_ratio += step;

                if (duty_ratio >= pwm_period)
                {
                    duty_ratio = pwm_period;
                    step = -1;
                }
                else if (duty_ratio <= 0)
                {
                    duty_ratio = 0;
                    step = +1;
                } 
        }
        
        else if (current_state == timer_countdown_nblink)
        {
            xSemaphoreTake(adc_value_sem, portMAX_DELAY);   // take mutex
            duty_ratio = (global_adc_value * pwm_period) / 1023;
            xSemaphoreGive(adc_value_sem);
        }
        
        
        
    }
}

// Timer 3 ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    
    counter++;

    if (counter < duty_ratio)
        LED2 = 1;    // ON portion
    else
        LED2 = 0;    // OFF portion

    if (counter >= pwm_period)
        counter = 0; // restart PWM cycle

    IFS0bits.T3IF = 0; // Clear Timer 3 interrupt flag
}