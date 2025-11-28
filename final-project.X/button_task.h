/* 
 * File:   button_task.h
 * Author: Bogdan
 *
 * Created on November 26, 2025, 9:41 AM
 */

#ifndef BUTTON_TASK_H
#define	BUTTON_TASK_H

extern states next_state;
extern states current_state;
extern pb0_last;
extern pb0_held;
extern pb0_click;

extern pb1_last;
extern pb1_held;
extern pb1_click;

extern pb2_last;
extern pb2_held;
extern pb2_click;

extern uint8_t pb_manager_flags;   // Bit-field of flags to be used by button manager logic
#define PB_UPDATE   0   // Indicates an IOC interrupt has occured
#define PB0_LAST    1   // Indicates the previous state of PB0
#define PB1_LAST    2   // Indicates the previous state of PB1
#define PB2_LAST    3   // Indicates the previous state of PB2
#define PB0_ON      4   // Indicates PB0 is currently pressed and its counter is running
#define PB1_ON      5   // Indicates PB1 is currently pressed and its counter is running
#define PB2_ON      6   // Indicates PB2 is currently pressed and its counter is running

//extern uint16_t pb0_time;   // Count of milliseconds that PB0 has been held for
//extern uint16_t pb1_time;   // Count of milliseconds that PB1 has been held for
//extern uint16_t pb2_time;   // Count of milliseconds that PB2 has been held for

void buttons_update(void);

void buttons_reset(void);

#endif	/* BUTTON_TASK_H */

