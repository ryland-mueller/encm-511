#include "common.h"


void do_adc_init(void)
{
    AD1CON2bits.PVCFG = 0;      // Selects Positive voltage reference
    AD1CON2bits.NVCFG0 = 0;     // Selects Negative voltage reference
    
    //Setup Channel 0 sample A
    AD1CHSbits.CH0NA = 0;       // Negative input is selected as gnd
    AD1CHSbits.CH0SA = 5;       // Positive input selected as AN5 (PIN7)
    
    AD1CON3bits.ADCS = 255;     // AD Conversion clock is set to 256 * Tcy 
    AD1CON1bits.SSRC = 7;       // ADC occurs based off SAMP 
    AD1CON1bits.FORM = 0;       // Data output is absolute decimal unsigned right justified
    AD1CON5bits.ASEN = 0;       // Disable auto scan
    AD1CON1bits.DMAEN = 0;      // Disable DMA 
    AD1CON2bits.SMPI = 0;       // Interrupts at the completion of each sample
    AD1CON1bits.MODE12 = 0;     // 10-bit ADC mode
    AD1CON1bits.ASAM = 0;       // Sampling starts when SAMP is set manually
    
    AD1CON1bits.ADON = 1;       // Enable ADC
    AD1CON1bits.SAMP = 0;       // Don't sample yet
}

void vDoAdcTask( void * pvParameters )
{
    char SetADCBuffer[4];
    TickType_t LastWakeTime;
    const TickType_t Frequency = 30;    // Perform an action every n ticks.
    
    LastWakeTime = xTaskGetTickCount(); // get current time.

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &LastWakeTime, Frequency );
        
        // Perform action here.
        
        xSemaphoreTake(adc_value_sem, portMAX_DELAY);   // take mutex
        xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
        
        AD1CON1bits.SAMP = 1;                   // start A/D conversion

        while (!AD1CON1bits.DONE){}             // wait for ADC read to finish

        AD1CON1bits.SAMP = 0;                   // stop conversion
        
        global_adc_value = ADC1BUF0;            // update global adc value
        
        for (const char *p = ADC_HOME; *p != '\0'; p++) {
            xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }
        
        for (const char *p = ADC_MESSAGE; *p != '\0'; p++) {
            xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }
        
        SetADCBuffer[4] = (global_adc_value % 10) + '0';
        SetADCBuffer[3] = ((global_adc_value/10) % 10) + '0';
        SetADCBuffer[2] = ((global_adc_value/100) % 10) + '0';
        SetADCBuffer[1] = ((global_adc_value/1000) % 10) + '0';
        SetADCBuffer[0] = ((global_adc_value/10000) % 10) + '0';
        
        for (const char *p = SetADCBuffer; *p != '\0'; p++) {
           xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }

        xSemaphoreGive(adc_value_sem);                  // give mutex
        xSemaphoreGive(uart_tx_queue_sem);  
    }
}
