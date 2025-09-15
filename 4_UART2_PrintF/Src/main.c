#include <stdint.h>
#include "stm32C0xx.h"



#define GPIOAEN		(1U<<0)
#define UART2EN		(1U<<17)

#define CR1_TE		(1U << 3)
#define CR1_UE		(1U << 0)

#define SR_TXE		(1U << 7)
#define CR_FIFOEN		(1U << 29)

#define SYS_FREQ	12000000
#define APB1_CLK	SYS_FREQ

#define UART_BAUDRATE	115200

static void uart_set_baudrate(USART_TypeDef * USARTx, uint32_t PeriphClk, uint32_t DesiredBaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);
void uart2_write(int ch);
void uart2_tx_init(void);


int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}

int main(void)
{
	uart2_tx_init();
	while(1)
	{
		printf("Hellooooo\n\r");

	}

}


void uart2_tx_init(void)
{
	//uart gpio pin
	//enable clock on gpioa
	RCC->IOPENR |= GPIOAEN;

	//PA2 mode to alternate function mode
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//SetPA2 alt function type to UART_TX (AF07)
	GPIOA->AFR[0] &= ~(0xFU << (2 * 4)); // clear AFRL bits for PA2
	GPIOA->AFR[0] |=  (0x1U << (2 * 4)); // AF1 = USART2_TX

	//configure UART module
	//enable clock on UART2
	RCC->APBENR1 |= UART2EN;
	//consigure BAUD rate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	//USART2->BRR =  (2 * 48000000)/921600;
	//config transfer direction
	USART2->CR1 = 0x0;
	USART2->CR1 = CR1_TE;
	USART2->CR1 &= ~CR_FIFOEN;
	//enable uArt module
	USART2->CR1 |= CR1_UE;


}

void uart2_write(int ch)
{
	//transmit data reg should be empty first
	while(!(USART2->ISR & SR_TXE)){};
	//write to transmit data reg
	USART2->TDR  = (ch & 0xFF);
	//

}

static void uart_set_baudrate(USART_TypeDef * USARTx, uint32_t PeriphClk, uint32_t DesiredBaudRate)
{
	USARTx->BRR = compute_uart_bd(PeriphClk, DesiredBaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
