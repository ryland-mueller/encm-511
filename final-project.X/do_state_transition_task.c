#include "common.h"

// prototypes
uint8_t ValidCharInput(void);

void do_state_transition_init(void)
{
    current_state = waiting_state;
    next_state = waiting_state;
}

void vDoStateTransitionTask( void * pvParameters )
{

    TickType_t LastWakeTime;
    const TickType_t Frequency = 100;    // Perform an action every n ticks.
    
    LastWakeTime = xTaskGetTickCount(); // get current time.

    uint8_t uart_input = 0;

    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &LastWakeTime, Frequency );
        
        // Perform action here.
        xSemaphoreTake(state_sem, portMAX_DELAY);     // take uart mutex

        switch (current_state)
        {
            case waiting_state:
                // Actions for waiting_state
                T2CONbits.TON = 0;
                xSemaphoreTake(uart_tx_queue_sem, portMAX_DELAY);     // take uart mutex
                // clear the screen because it looks better on startup/reset
                for (const char *p = CLEAR_SCREEN; *p != '\0'; p++) {
                    xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
                }
                // send the cursor to the message line
                for (const char *p = MESSAGE_HOME; *p != '\0'; p++) {
                    xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
                }
                //Display the welcom message
                for (const char *p = WELCOME_MESSAGE; *p != '\0'; p++) {
                    xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
                }
                xSemaphoreGive(uart_tx_queue_sem);
                // Transition logic
                if (pb_stat == PB0_CLICKED){
                    next_state = set_timer;
                }
                break;

            case set_timer:
                // Actions for set_timer
                //LED0 = 1;
                // Transition logic
                if (pb_stat == PB1_PB2_CLICKED){
                    xQueueReset(xUartReceiveQueue); // reset queue after set_time has been locked in
                    next_state = timer_countdown;
                }
                break;

            case timer_countdown:
                // Actions for timer_countdown
                T2CONbits.TON = 1;                  // run the countdown
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_paused;
                }
                else if (xTaskNotifyWait( 0UL, ULONG_MAX, NULL, 0UL ) == pdPASS); // check if timer expired
                {
                    next_state = timer_finished;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_countdown_info;
                    else if (uart_input == 'b')
                        next_state = timer_countdown_nblink;
                }
                break;

            case timer_paused:
                // Actions for timer_paused
                T2CONbits.TON = 0;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_countdown;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_info_paused;
                    else if (uart_input == 'b')
                        next_state = timer_nblink_paused;
                }
                break;

            case timer_countdown_info:
                // Actions for timer_countdown_info
                T2CONbits.TON = 1;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_info_paused;
                }
                else if (xTaskNotifyWait( 0UL, ULONG_MAX, NULL, 0UL ) == pdPASS); // check if timer expired
                {
                    next_state = timer_finished;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_countdown;
                    else if (uart_input == 'b')
                        next_state = timer_countdown_info_nblink;
                }
                break;

            case timer_info_paused:
                // Actions for timer_info_paused
                T2CONbits.TON = 1;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_countdown_info;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_paused;
                    else if (uart_input == 'b')
                        next_state = timer_info_nblink_paused;
                }
                break;

            case timer_countdown_nblink:
                // Actions for timer_countdown_nblink
                T2CONbits.TON = 1;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_nblink_paused;
                }
                else if (xTaskNotifyWait( 0UL, ULONG_MAX, NULL, 0UL ) == pdPASS); // check if timer expired
                {
                    next_state = timer_finished;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_countdown_info_nblink;
                    else if (uart_input == 'b')
                        next_state = timer_countdown;
                }
                break;

            case timer_nblink_paused:
                // Actions for timer_nblink_paused
                T2CONbits.TON = 0;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_countdown_info_nblink;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_info_nblink_paused;
                    else if (uart_input == 'b')
                        next_state = timer_paused;
                }
                break;

            case timer_countdown_info_nblink:
                // Actions for timer_countdown_info_nblink
                T2CONbits.TON = 1;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_info_nblink_paused;
                }
                else if (xTaskNotifyWait( 0UL, ULONG_MAX, NULL, 0UL ) == pdPASS); // check if timer expired
                {
                    next_state = timer_finished;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_countdown_nblink;
                    else if (uart_input == 'b')
                        next_state = timer_countdown_info;
                }
                break;

            case timer_info_nblink_paused:
                // Actions for timer_info_nblink_paused
                T2CONbits.TON = 0;
                // Transition logic

                // must do this first otherwise clicked flag will be missed
                if (pb_stat == PB2_CLICKED){
                    next_state = timer_countdown_info_nblink;
                }
                else
                {
                    uart_input = ValidCharInput();          // check the next UART char 
                    if (uart_input == 'i')
                        next_state = timer_nblink_paused;
                    else if (uart_input == 'b')
                        next_state = timer_info_paused;
                }
                break;

            case timer_finished:
                // Actions for timer_finished
                LastWakeTime = xTaskGetTickCount(); // get current time.

                // Transition logic
                vTaskDelayUntil( &LastWakeTime, 5000 );
                next_state = waiting_state;
                break;

            default:
                // Handle unexpected state
                // Transition logic
                next_state = waiting_state;
                break;
        }

    // Update current state at the end of the cycle
    pb_stat = 0;

    if(current_state != next_state){
        xSemaphoreTake(uart_tx_queue_sem,portMAX_DELAY);
        for (const char *p = CLEAR_SCREEN; *p != '\0'; p++) {
            xQueueSendToBack(xUartTransmitQueue, p, portMAX_DELAY);
        }
        xSemaphoreGive(uart_tx_queue_sem);
    }
    current_state = next_state;
    xSemaphoreGive(state_sem);


    }
}

uint8_t ValidCharInput(void) {
    // read the xUartReceiveQueue until there is an 'i' or 'b' or no more chars
    uint8_t char_to_read = 0;
    while (xQueueReceive(xUartReceiveQueue, &char_to_read, 0) == pdTRUE) {
        if (char_to_read == 'i') {
            return 'i';   // ASCII code for 'i'
        } else if (char_to_read == 'b') {
            return 'b';   // ASCII code for 'b'
        }
    }
    return 0; // return a default value if neither 'i' nor 'b' found
}
