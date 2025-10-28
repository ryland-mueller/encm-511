/* 
 * File:   buttons.h
 * Author: Ryland
 *
 * Created on October 23, 2025, 6:37 PM
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

extern uint8_t pb_manager_flags;   // Bit-field of flags to be used by button manager logic
#define PB_UPDATE   0   // Indicates an IOC interrupt has occured
#define PB0_LAST    1   // Indicates the previous state of PB0
#define PB1_LAST    2   // Indicates the previous state of PB1
#define PB2_LAST    3   // Indicates the previous state of PB2
#define PB0_ON      4   // Indicates PB0 is currently pressed and its counter is running
#define PB1_ON      5   // Indicates PB1 is currently pressed and its counter is running
#define PB2_ON      6   // Indicates PB2 is currently pressed and its counter is running

extern uint16_t pb0_time;   // Count of milliseconds that PB0 has been held for
extern uint16_t pb1_time;   // Count of milliseconds that PB1 has been held for
extern uint16_t pb2_time;   // Count of milliseconds that PB2 has been held for

void buttons_update(void);

void buttons_reset(void);

#endif	/* BUTTONS_H */

