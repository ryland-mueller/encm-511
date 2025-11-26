#include "common.h"

void vDoAdcTask( void * pvParameters )
{
    TickType_t LastWakeTime;
    const TickType_t Frequency = 10;    // Perform an action every n ticks.

    LastWakeTime = xTaskGetTickCount(); // get current time.

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &LastWakeTime, Frequency );
        
        // Perform action here.
        
        xSemaphoreTake(adc_value_sem, portMAX_DELAY);   // take mutex
                
        AD1CON1bits.SAMP = 1;                   // start A/D conversion

        while (!AD1CON1bits.DONE){}             // wait for ADC read to finish

        AD1CON1bits.SAMP = 0;                   // stop conversion
        
        global_adc_value = ADC1BUF0;            // update global adc value
        
        xSemaphoreGive(adc_value_sem);                  // give mutex
    
    }
}
