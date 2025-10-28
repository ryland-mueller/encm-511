/* 
 * File:   ADC.h
 * Author: Ryland Mueller, Patrick Mitenko, Bogdan Bacea
 *
 * Created on October 28, 2025, 7:22 AM
 */

#ifndef ADC_H
#define	ADC_H

#include "stdint.h"

void adc_init(void);

uint16_t do_adc(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */
