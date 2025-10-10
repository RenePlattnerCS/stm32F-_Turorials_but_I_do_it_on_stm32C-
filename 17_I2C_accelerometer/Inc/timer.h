/*
 * timer.h
 *
 *  Created on: Sep 30, 2025
 *      Author: NeerW
 */

#ifndef TIMER_H_
#define TIMER_H_

void tim1_1hz_init(void);
void tim1_output_compare(void);
void tim3_pa6_input_capture(void);
void tim1_1hz_interrupt(void);

#define SR_UIF		(1U << 0)
#define SR_CC1IF	(1U << 0)

#endif /* TIMER_H_ */
