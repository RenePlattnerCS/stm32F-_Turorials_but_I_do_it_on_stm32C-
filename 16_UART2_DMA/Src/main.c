#include <stdint.h>
#include <stdio.h>
#include "stm32C0xx.h"
#include "uart.h"

#define GPIOAEN		(1U << 0)
#define GPIOA_5		(1U << 5)
#define LED_PIN 	GPIOA_5

char key;

int main(void)
{
	RCC->IOPENR |= GPIOAEN;
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);

	uart2_rxtx_interrupt();

	while(1)
	{

	}

}



void USART2_IRQHandler(void)
{
	//check if something set in Read status reg
	if(USART2->ISR & SR_RXNE)
	{
		key = USART2->RDR;

		if(key == '1')
				{
					GPIOA->ODR |= LED_PIN;
				}
				else
				{
					GPIOA->ODR &= ~LED_PIN;
				}
	}
}

