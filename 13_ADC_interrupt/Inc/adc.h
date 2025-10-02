/*
 * adc.h
 *
 *  Created on: Sep 28, 2025
 *      Author: NeerW
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

uint32_t adc_read(void);
void start_conversion(void);
void pa1_adc_interrupt(void);

#define ADC_SR_EOC			(1U << 2)

#endif /* ADC_H_ */
