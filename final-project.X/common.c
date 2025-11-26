
#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_tx_sem = NULL;       // mutex to use uart (tx and rx)
QueueHandle_t xUartTransmitQueue = NULL;    // queue to transmit on the UART

uint16_t global_adc_value = 0;              // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex
