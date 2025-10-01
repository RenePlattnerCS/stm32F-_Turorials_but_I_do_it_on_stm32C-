

#ifndef EXTI_H_
#define EXTI_H_

#include "stm32C0xx.h"

#define LINE13 		(1U<<13) // pending register EXTI_FPR1


void pc13_exti_init(void);
#endif /* EXTI_H_ */
