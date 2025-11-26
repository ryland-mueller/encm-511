
#include "common.h"

// Define globals here (only once)
SemaphoreHandle_t uart_sem = NULL;          // mutex to use uart (tx and rx)

uint16_t global_adc_value = 0;              // global ADC value
SemaphoreHandle_t adc_value_sem = NULL;     // and its mutex
