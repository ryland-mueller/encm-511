
#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_tx_sem = NULL;       // mutex to use uart tx
QueueHandle_t xUartTransmitQueue = NULL;    // queue to transmit on the UART

SemaphoreHandle_t uart_rx_sem = NULL;       // mutex to use uart rx
QueueHandle_t xUartRecieveQueue = NULL;    // queue to transmit on the UART


uint16_t global_adc_value = 0;              // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex

void StringToQueue(char *String)
{
    uint8_t char_to_send = 0;
    
    for(uint8_t string_index = 0; string_index <= strlen(String); string_index++)
    {
        char_to_send = String[string_index];
        xQueueSendToBack(xUartTransmitQueue, (void*)&char_to_send, portMAX_DELAY);
    }
    
    
}