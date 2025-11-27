
#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_tx_sem = NULL;       // mutex to use uart tx
QueueHandle_t xUartTransmitQueue = NULL;    // queue to transmit on the UART

SemaphoreHandle_t uart_rx_sem = NULL;       // mutex to use uart rx
QueueHandle_t xUartRecieveQueue = NULL;    // queue to transmit on the UART


uint16_t global_adc_value = 0;              // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex


extern TaskHandle_t DoUartAdcTaskHandle = 0;
extern TaskHandle_t DoUartTransmitTaskHandle = 0;
extern TaskHandle_t DoUartRecieveTaskHandle = 0;