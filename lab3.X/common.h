/* 
 * File:   common.h
 * Author: Ryland
 *
 * Created on October 23, 2025, 11:26 AM
 */

#ifndef COMMON_H
#define	COMMON_H

#define SET_BIT(flags, n)       ((flags) |= (1 << (n)))
#define CHECK_BIT(flags, n)     (((flags) >> (n)) & 1)
#define CLEAR_BIT(flags, n)     ((flags) &= ~(1 << (n)))
#define TOGGLE_BIT(flags, n)    ((flags) ^= (1 << (n)))

extern uint8_t pb_stat;        // Bit-field for button status flags
#define PB0_HELD_FLAG    0   // Flag set to indicate PB0 is currently being held (after being held for >1s)
#define PB0_CLICKED_FLAG 1   // Flag set to indicate PB0 has been clicked. Consumer should clear flag once acting on it
#define PB1_HELD_FLAG    2   // Flag set to indicate PB1 is currently being held (after being held for >1s)
#define PB1_CLICKED_FLAG 3   // Flag set to indicate PB1 has been clicked. Consumer should clear flag once acting on it
#define PB2_HELD_FLAG    4   // Flag set to indicate PB2 is currently being held (after being held for >1s)
#define PB2_CLICKED_FLAG 5   // Flag set to indicate PB2 has been clicked. Consumer should clear flag once acting on it

// Define the transition combination to compare with pb_stat
#define PB0_CLICKED         (1 << PB0_CLICKED_FLAG)
#define PB1_CLICKED         (1 << PB1_CLICKED_FLAG)
#define PB2_CLICKED         (1 << PB2_CLICKED_FLAG)
#define PB0_HELD            (1 << PB0_HELD_FLAG)
#define PB1_HELD            (1 << PB1_HELD_FLAG)
#define PB2_HELD            (1 << PB2_HELD_FLAG)
#define PB0_PB1_HELD       ((1 << PB0_HELD_FLAG) | \
                            (1 << PB1_HELD_FLAG))
#define PB0_PB2_HELD       ((1 << PB0_HELD_FLAG) | \
                            (1 << PB2_HELD_FLAG))
#define PB1_PB2_HELD       ((1 << PB1_HELD_FLAG) | \
                            (1 << PB2_HELD_FLAG))
#define PB0_PB1_PB2_HELD   ((1 << PB0_HELD_FLAG) | \
                            (1 << PB1_HELD_FLAG) | \
                            (1 << PB2_HELD_FLAG))

#endif	/* COMMON_H */

