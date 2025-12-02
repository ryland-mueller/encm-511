#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_tx_sem = NULL;       // mutex to use uart tx
QueueHandle_t xUartTransmitQueue = NULL;    // queue to transmit on the UART

SemaphoreHandle_t uart_rx_sem = NULL;       // mutex to use uart rx
QueueHandle_t xUartReceiveQueue = NULL;     // queue to transmit on the UART

uint16_t global_adc_value = 0;           // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex

uint16_t set_time;                   //Global variable that holds the set time by the user
SemaphoreHandle_t countdown_sem = NULL;     //Mutex for safely changing the countdown seconds

SemaphoreHandle_t uart_tx_queue_sem = NULL;

SemaphoreHandle_t state_sem = NULL;

extern TaskHandle_t DoUartAdcTaskHandle = NULL;
extern TaskHandle_t DoUartTransmitTaskHandle = NULL;
extern TaskHandle_t DoUartRecieveTaskHandle = NULL;
extern TaskHandle_t DoTimerTaskHandle = NULL;
extern TaskHandle_t SetTimerTaskHandle = NULL;
extern TaskHandle_t DoButtonTaskHandle = NULL;
extern TaskHandle_t DoStateTransitionHandle = NULL;

extern states current_state = waiting_state;
extern states next_state = waiting_state;
extern TaskHandle_t DoLED2TaskHandle = NULL;
extern TaskHandle_t DoLED01Handle = NULL;



