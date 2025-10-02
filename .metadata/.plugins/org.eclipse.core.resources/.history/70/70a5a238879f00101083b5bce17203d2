#include <stdint.h>
#include <stdio.h>
#include "stm32C0xx.h"
#include "uart.h"
#include "timer.h"

#include "systick.h"



int timestamp = 0;

int main(void)
{


	tim1_output_compare();
	tim3_pa6_input_capture();

	while(1)
	{
		while(!(TIM3->SR & SR_CC1IF)){}

		timestamp = TIM3->CCR1;

	}

}





