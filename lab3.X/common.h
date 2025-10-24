/*
 * File:   common.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created FOR ENCM 511
 * PLEASE ADD DATE CREATED HERE: 2025-10-23
 */

#ifndef COMMON_H
#define	COMMON_H

#include "xc.h"

#define SET_BIT(flags, n)       ((flags) |= (1 << (n)))
#define CHECK_BIT(flags, n)     (((flags) >> (n)) & 1)
#define CLEAR_BIT(flags, n)     ((flags) &= ~(1 << (n)))
#define TOGGLE_BIT(flags, n)    ((flags) ^= (1 << (n)))

extern uint8_t pb_stat;      // Bit-field for button status flags
#define PB0_HELD_FLAG    0   // Flag set to indicate PB0 is being held and program has not acted on that yet
#define PB0_CLICKED_FLAG 1   // Flag set to indicate PB0 has been clicked
#define PB1_HELD_FLAG    2   // Flag set to indicate PB1 is being held and program has not acted on that yet
#define PB1_CLICKED_FLAG 3   // Flag set to indicate PB1 has been clicked
#define PB2_HELD_FLAG    4   // Flag set to indicate PB2 is being held and program has not acted on that yet
#define PB2_CLICKED_FLAG 5   // Flag set to indicate PB2 has been clicked

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

