#include "uart.h"

#define GPIOAEN		(1U<<0)
#define UART2EN		(1U<<17)

#define CR1_TE		(1U << 3)
#define CR1_RE		(1U << 2)
#define CR1_UE		(1U << 0)

#define SR_TXE		(1U << 7)
#define SR_RXNE  	(1U << 5)
#define CR_FIFOEN		(1U << 29)

#define SYS_FREQ	12000000
#define APB1_CLK	SYS_FREQ

#define UART_BAUDRATE	115200

#define CR1_RXNEIE		(1U << 5)
#define CR1_TE	(1U << 3)

#define AHB_DMA1EN		(1U << 0)
#define DMA_S_EN		(1U << 0)

static void uart_set_baudrate(USART_TypeDef * USARTx, uint32_t PeriphClk, uint32_t DesiredBaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);
void uart2_write(int ch);



int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}



//stm32C0 uses simpler DMA architecture => no streams just channels!
void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len)
{
	//enable clock access to dma
	RCC->AHBENR |= AHB_DMA1EN;
	//disable dma1 stream 6
	DMA1_Stream6->CR &=~DMA_S_EN;
	//clear all interrupt flags of stream6

	//set the source buffer
	//set length
	//select stream6 ch4

	//enable memory increment

	// memory -> to peripheral , transfer direction

	// enable direct mode , disable FIFO

	//enable dma1 stream6
	//enable UART2 transmitter MDA
	// DMA interrupt enable NVIC




}


void uart2_rxtx_init(void)
{
	//uart gpio pin
	//enable clock on gpioa
	RCC->IOPENR |= GPIOAEN;

	//PA2 mode to alternate function mode
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//SetPA2 alt function type to UART_TX (AF01)
	GPIOA->AFR[0] &= ~(0xFU << (2 * 4)); // clear AFRL bits for PA2
	GPIOA->AFR[0] |=  (0x1U << (2 * 4)); // AF1 = USART2_TX



	//RECEIVE
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);
	GPIOA->AFR[0] &= ~(0xFU << (12)); // clear AFRL bits for PA2
	GPIOA->AFR[0] |=  (0x1U << (3 * 4)); // AF1 = USART2_TX



	//configure UART module
	//enable clock on UART2
	RCC->APBENR1 |= UART2EN;
	//consigure BAUD rate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	//USART2->BRR =  (2 * 48000000)/921600;
	//config transfer direction -> enable receiver
	USART2->CR1 = 0x0;
	USART2->CR1 = (CR1_TE | CR1_RE);
	USART2->CR1 &= ~CR_FIFOEN;
	//enable uArt module
	USART2->CR1 |= CR1_UE;


}



void uart2_tx_init(void)
{
	//uart gpio pin
	//enable clock on gpioa
	RCC->IOPENR |= GPIOAEN;

	//PA2 mode to alternate function mode
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//SetPA2 alt function type to UART_TX (AF01)
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



char uart2_read(void)
{
	//check receive reg not empty
	while(!(USART2->ISR & SR_RXNE)){};

	return USART2->RDR;
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




void uart2_rxtx_interrupt(void)
{
	//uart gpio pin
	//enable clock on gpioa
	RCC->IOPENR |= GPIOAEN;

	//PA2 mode to alternate function mode
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//SetPA2 alt function type to UART_TX (AF01)
	GPIOA->AFR[0] &= ~(0xFU << (2 * 4)); // clear AFRL bits for PA2
	GPIOA->AFR[0] |=  (0x1U << (2 * 4)); // AF1 = USART2_TX



	//RECEIVE
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);
	GPIOA->AFR[0] &= ~(0xFU << (12)); // clear AFRL bits for PA2
	GPIOA->AFR[0] |=  (0x1U << (3 * 4)); // AF1 = USART2_TX



	//configure UART module
	//enable clock on UART2
	RCC->APBENR1 |= UART2EN;
	//consigure BAUD rate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	//USART2->BRR =  (2 * 48000000)/921600;
	//config transfer direction -> enable receiver
	USART2->CR1 = 0x0;
	USART2->CR1 = (CR1_TE | CR1_RE);
	USART2->CR1 &= ~CR_FIFOEN;

	//USART2->CR1 |= CR1_RXNEIE; //enable interrupt
	USART2->CR1 |= CR1_RXNEIE | CR1_TE;
	//enable UART2 interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);

	//enable uArt module
	USART2->CR1 |= CR1_UE;


}

