#include "common.h"

uint32_t duty_ratio_tick;       //Holds the on value for the LED in terms of timer counts
uint32_t duty_ratio_percent;    //Holds the percentage of the duty ratio to send over UART
uint8_t pwm_period = 10;        //How many steps the LED PWM has
int8_t step = 1;                //Used to pulse the eld
static uint16_t counter = 0;    //Increments whenever timer goes off, used to determine on amount of LED 
uint8_t blink_enable = 1;       // whether LED is allowed to output PWM
uint16_t blink_counter = 0;     // simple blink timer


void do_timer3_init(void)
{ 
    //Timer 3 setup
    T2CONbits.T32 = 0;              // Operate timers 2 & 3 as separate 16-bit timers
    
    T3CONbits.TON = 0;              //Turn timer 3 off
    T3CONbits.TCKPS = 0;            // set prescaler to 1:1
    IPC2bits.T3IP = ISR_PRIORITY;   // Interrupt priority
    IFS0bits.T3IF = 0;              // clear interrupt flag
    IEC0bits.T3IE = 1;              // enable interrupt
    PR3 = 8000;                     // set period for 2ms
    T3CONbits.TON = 1;              //Turn timer 3 on
    return;
}

void vDoLED2Task(void *pvParameters)
{
    TickType_t LastWakeTime;        
    LastWakeTime = xTaskGetTickCount(); //get current tick
    TickType_t frequency = 100;         //frequency of the task
    uint16_t local_adc_value = 0;       //Used to store the global adc value locally
    char DRbuffer[4];                   //buffer to send the duty ratio over UART
    
    for(;;)
    {
        vTaskDelayUntil( &LastWakeTime, pdMS_TO_TICKS(150) );
        
        xSemaphoreTake(adc_value_sem, portMAX_DELAY);   //take adc_value mutex
        local_adc_value = global_adc_value;             //save the value locally to give up the mutex
        xSemaphoreGive(adc_value_sem);                  //Give adc_value mutex
        
        blink_counter += frequency;  //increment the blink counter every time the task runs
        if (blink_counter >= 500) 
        {
            blink_counter = 0;
            blink_enable ^= 1;       //toggle blink ON/OFF
        }
        
        if (current_state == waiting_state)
        {
            T3CONbits.TON = 1;
            // Breathe the led
            duty_ratio_tick += step; // increase the duty ratio tick by the step amount

                if (duty_ratio_tick >= pwm_period)  //when the duty ratio gets to 10 turn the step to -1
                {
                    duty_ratio_tick = pwm_period;
                    step = -1;
                }
                else if (duty_ratio_tick <= 0)  //when the duty ratio gets to zero turn the step to +1
                {
                    duty_ratio_tick = 0;
                    step = +1;
                } 
        }
        
        else if (current_state == timer_countdown_nblink
                || current_state == timer_finished
                || current_state == timer_nblink_paused)
        {
            T3CONbits.TON = 1;
            duty_ratio_tick = (local_adc_value * pwm_period) / 1023;
        }
        else if (current_state == timer_countdown_info_nblink
                || current_state == timer_info_nblink_paused)
        {
            xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
            T3CONbits.TON = 1;
            duty_ratio_tick = (local_adc_value * pwm_period) / 1023;
            // display duty ratio on uart
            duty_ratio_percent = (local_adc_value * (uint32_t) 100 ) / 1023;
            
            DRbuffer[0] = (duty_ratio_percent /    100) + '0';         // hundreds
            DRbuffer[1] = ((duty_ratio_percent /    10) % 10) + '0';   // tens
            DRbuffer[2] = (duty_ratio_percent %     10) + '0';         // ones
            DRbuffer[3] = '%';
            DRbuffer[4] = '\0';
            //Send the command to set the cursor to Duty Ratio home
            for (const char *p = DR_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the duty ratio message
            for (const char *p = DUTY_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the current duty ratio
            for (const char *p = DRbuffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            xSemaphoreGive(uart_tx_queue_sem);
        }
        else if (current_state == timer_countdown_info)
        {
            xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
            T3CONbits.TON = 1;
            //Determine if the task is in the on or off half of the blink ratio
            if (blink_enable)
                duty_ratio_tick = (local_adc_value * pwm_period) / 1023;  // normal brightness
            else
                duty_ratio_tick = 0;  //Sets the duty ratio to zero to turn off the led for this half
            
            // display duty ratio on uart
            duty_ratio_percent = (local_adc_value * (uint32_t) 100 ) / 1023;

            DRbuffer[0] = (duty_ratio_percent /    100) + '0';         // hundreds
            DRbuffer[1] = ((duty_ratio_percent /    10) % 10) + '0';   // tens
            DRbuffer[2] = (duty_ratio_percent %     10) + '0';         // ones
            DRbuffer[3] = '%';
            DRbuffer[4] = '\0';
            //Send the command to set the cursor to Duty Ratio home
            for (const char *p = DR_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the duty ratio message
            for (const char *p = DUTY_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the current duty ratio
            for (const char *p = DRbuffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            xSemaphoreGive(uart_tx_queue_sem);
        }
        else if (current_state == timer_countdown)
        {
            T3CONbits.TON = 1;
            //Determine if the task is in the on or off half of the blink ratio
            if (blink_enable)
                duty_ratio_tick = (local_adc_value * pwm_period) / 1023;  // normal brightness
            else
                duty_ratio_tick = 0;  //Sets the duty ratio to zero to turn off the led for this half
        }
        else if (current_state == timer_info_paused)
        {
            xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
            T3CONbits.TON = 0;
            duty_ratio_tick = (local_adc_value * pwm_period) / 1023;
            // display duty ratio on uart
            duty_ratio_percent = (local_adc_value * (uint32_t) 100 ) / 1023;
            
            DRbuffer[0] = (duty_ratio_percent /    100) + '0';         // hundreds
            DRbuffer[1] = ((duty_ratio_percent /    10) % 10) + '0';   // tens
            DRbuffer[2] = (duty_ratio_percent %     10) + '0';         // ones
            DRbuffer[3] = '%';
            DRbuffer[4] = '\0';
            //Send the command to set the cursor to Duty Ratio home
            for (const char *p = DR_HOME; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the duty ratio message
            for (const char *p = DUTY_MESSAGE; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            //Send the current duty ratio
            for (const char *p = DRbuffer; *p != '\0'; p++) {
                xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
            }
            xSemaphoreGive(uart_tx_queue_sem); 
        }     
        else
        {
            T3CONbits.TON = 0;
            LED2 = 0;
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