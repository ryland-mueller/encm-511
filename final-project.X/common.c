
#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_tx_sem = NULL;       // mutex to use uart tx
QueueHandle_t xUartTransmitQueue = NULL;    // queue to transmit on the UART

SemaphoreHandle_t uart_rx_sem = NULL;       // mutex to use uart rx
QueueHandle_t xUartRecieveQueue = NULL;     // queue to transmit on the UART


uint16_t global_adc_value = 0;           // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex

uint16_t set_time;                   //Global variable that holds the set time by the user
SemaphoreHandle_t countdown_sem;     //Mutex for safely changing the countdown seconds

extern TaskHandle_t DoUartAdcTaskHandle = NULL;
extern TaskHandle_t DoUartTransmitTaskHandle = NULL;
extern TaskHandle_t DoUartRecieveTaskHandle = NULL;
extern TaskHandle_t DoTimerTaskHandle = NULL;