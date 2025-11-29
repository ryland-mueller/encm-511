#include "common.h"

uint32_t duty_ratio_tick;
uint32_t duty_ratio_percent;
uint8_t pwm_period = 10;
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
    uint16_t local_adc_value = 0;
    char DRbuffer[4];
    
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, frequency );
        
        xSemaphoreTake(adc_value_sem, portMAX_DELAY);   // take mutex
        local_adc_value = global_adc_value;    
        xSemaphoreGive(adc_value_sem);
        
        if (current_state == waiting_state)
        {
            // Breath the led
            duty_ratio_tick += step;

                if (duty_ratio_tick >= pwm_period)
                {
                    duty_ratio_tick = pwm_period;
                    step = -1;
                }
                else if (duty_ratio_tick <= 0)
                {
                    duty_ratio_tick = 0;
                    step = +1;
                } 
        }
        
        else if (current_state == timer_countdown_nblink)
        {
            duty_ratio_tick = (local_adc_value * pwm_period) / 1023;
            
        }
        
        else if (current_state == timer_countdown_info_nblink)
        {
            xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
            duty_ratio_tick = (local_adc_value * pwm_period) / 1023;
            // display duty ratio on uart
            duty_ratio_percent = (local_adc_value * (uint32_t) 100 ) / 1023;
            
            DRbuffer[0] = (duty_ratio_percent /    100) + '0';         // hundreds
            DRbuffer[1] = ((duty_ratio_percent /    10) % 10) + '0';   // tens
            DRbuffer[2] = (duty_ratio_percent %     10) + '0';         // ones
            DRbuffer[3] = '%';
            DRbuffer[4] = '\0';
            
            for (const char *p = DR_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            
            for (const char *p = DUTY_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            
            for (const char *p = DRbuffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            xSemaphoreGive(uart_tx_queue_sem);
        }
        
        
        
    }
}

// Timer 3 ISR
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    
    counter++;

    if (counter < duty_ratio_tick)
        LED2 = 1;    // ON portion
    else
        LED2 = 0;    // OFF portion

    if (counter >= pwm_period)
        counter = 0; // restart PWM cycle

    IFS0bits.T3IF = 0; // Clear Timer 3 interrupt flag
}