#include "exti.h"

#define GPIOCEN		(1U<<2)
#define SYSCFGEN	(1U<<0)


void pc13_exti_init(void)
{
	//disable global interrupt
	__disable_irq();
	//enable clock GPIOC
	RCC->IOPENR |= GPIOCEN;
	//enable clock access to SYSCONFIG
	RCC->APBENR2 |= SYSCFGEN;

	//set pc13 as input
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);

	//select PORTC for EXTI13
	// 4. Map EXTI13 to Port C in SYSCFG_EXTICR4
	EXTI->EXTICR[3] &= ~(0xFU << 4);  // clear EXTI13 bits
	EXTI->EXTICR[3] |=  (0x2U << 4);  // 0x2 = Port C


	//unmask EXTI13
	EXTI->IMR1 |= (1U<<13);

	//select fallign edge trigger
	EXTI->FTSR1 |= (1U<<13);
	//disable rising
	EXTI->RTSR1 &= ~(1U << 13);  // Disable rising edge

	//enable exti line in NVIC
	//NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	//enable global interrupt
	__enable_irq();




}
