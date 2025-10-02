#include <stdint.h>
#include <stdio.h>
#include "stm32C0xx.h"
#include "uart.h"

#include "systick.h"

#define GPIOAEN		(1U<<0)

#define PIN5		(1U<<5)
#define LED_PIN		PIN5


int main(void)
{

	RCC->IOPENR |= GPIOAEN;


	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);


	uart2_tx_init();
	systick_1hz_interrupt();

	while(1)
	{



		//systickDelayMs(1000);

	}

}


void SysTick_Handler(void)
{
	printf("A second passed! \n\r");
	GPIOA->ODR ^= LED_PIN;
}



