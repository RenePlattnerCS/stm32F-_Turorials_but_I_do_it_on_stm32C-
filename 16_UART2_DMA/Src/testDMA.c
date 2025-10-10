// STM32C031C6 DMA Configuration for USART2
// Note: STM32C0 uses CHANNELS (not streams like STM32F4)

#include "stm32c0xx.h"

// DMA Register Bit Definitions
#define DMA_CCR_EN          (1U << 0)   // Channel enable
#define DMA_CCR_TCIE        (1U << 1)   // Transfer complete interrupt enable
#define DMA_CCR_HTIE        (1U << 2)   // Half transfer interrupt enable
#define DMA_CCR_TEIE        (1U << 3)   // Transfer error interrupt enable
#define DMA_CCR_DIR         (1U << 4)   // Data transfer direction (0=periph->mem, 1=mem->periph)
#define DMA_CCR_CIRC        (1U << 5)   // Circular mode
#define DMA_CCR_PINC        (1U << 6)   // Peripheral increment mode
#define DMA_CCR_MINC        (1U << 7)   // Memory increment mode
#define DMA_CCR_PSIZE_8     (0U << 8)   // Peripheral size 8-bit
#define DMA_CCR_MSIZE_8     (0U << 10)  // Memory size 8-bit
#define DMA_CCR_PL_LOW      (0U << 12)  // Priority level: Low
#define DMA_CCR_PL_MEDIUM   (1U << 12)  // Priority level: Medium
#define DMA_CCR_PL_HIGH     (2U << 12)  // Priority level: High
#define DMA_CCR_PL_VERY_HIGH (3U << 12) // Priority level: Very High
#define DMA_CCR_MEM2MEM     (1U << 14)  // Memory to memory mode

#define DMA_ISR_GIF4        (1U << 12)  // Channel 4 Global interrupt flag
#define DMA_ISR_TCIF4       (1U << 13)  // Channel 4 Transfer complete flag
#define DMA_ISR_HTIF4       (1U << 14)  // Channel 4 Half transfer flag
#define DMA_ISR_TEIF4       (1U << 15)  // Channel 4 Transfer error flag

#define DMA_IFCR_CGIF4      (1U << 12)  // Channel 4 Clear global interrupt flag
#define DMA_IFCR_CTCIF4     (1U << 13)  // Channel 4 Clear transfer complete flag
#define DMA_IFCR_CHTIF4     (1U << 14)  // Channel 4 Clear half transfer flag
#define DMA_IFCR_CTEIF4     (1U << 15)  // Channel 4 Clear transfer error flag

#define AHB_DMA1EN          (1U << 0)   // DMA1 clock enable bit

// USART2 CR3 register bits
#define USART_CR3_DMAT      (1U << 7)   // DMA enable transmitter

// Global variables
volatile uint8_t dma_tx_complete = 0;

/**
 * @brief Initialize DMA1 Channel 4 for USART2 TX
 * @param src: Source address (memory buffer)
 * @param dst: Destination address (USART2->TDR)
 * @param len: Number of bytes to transfer
 *
 * DMA Channel Mapping for STM32C0:
 * - USART2_TX uses DMA1 Channel 4
 * - USART2_RX uses DMA1 Channel 5
 */
void dma1_ch4_uart2_tx_init(uint32_t src, uint32_t dst, uint32_t len)
{
    // 1. Enable clock access to DMA1
    RCC->AHBENR |= AHB_DMA1EN;

    // 2. Disable DMA1 Channel 4 before configuration
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;

    // Wait until channel is disabled
    while(DMA1_Channel4->CCR & DMA_CCR_EN);

    // 3. Clear all interrupt flags for Channel 4
    DMA1->IFCR |= DMA_IFCR_CGIF4;   // Clear global interrupt flag
    DMA1->IFCR |= DMA_IFCR_CTCIF4;  // Clear transfer complete flag
    DMA1->IFCR |= DMA_IFCR_CHTIF4;  // Clear half transfer flag
    DMA1->IFCR |= DMA_IFCR_CTEIF4;  // Clear transfer error flag

    // 4. Configure DMA Channel 4
    DMA1_Channel4->CCR = 0;  // Clear all configuration bits

    // Set peripheral address (USART2 TDR register)
    DMA1_Channel4->CPAR = dst;  // Peripheral address: USART2->TDR

    // Set memory address (source buffer)
    DMA1_Channel4->CMAR = src;  // Memory address: your data buffer

    // Set number of data items to transfer
    DMA1_Channel4->CNDTR = len;

    // Configure channel settings
    DMA1_Channel4->CCR |= DMA_CCR_MINC;      // Memory increment mode enabled
    DMA1_Channel4->CCR &= ~DMA_CCR_PINC;     // Peripheral increment disabled (always write to TDR)
    DMA1_Channel4->CCR |= DMA_CCR_DIR;       // Direction: Memory to Peripheral
    DMA1_Channel4->CCR |= DMA_CCR_TCIE;      // Transfer complete interrupt enable
    DMA1_Channel4->CCR |= DMA_CCR_PL_HIGH;   // Priority level: High

    // Data size: 8-bit for both memory and peripheral
    DMA1_Channel4->CCR |= DMA_CCR_MSIZE_8;   // Memory size: 8-bit
    DMA1_Channel4->CCR |= DMA_CCR_PSIZE_8;   // Peripheral size: 8-bit

    // Optional: Enable circular mode for continuous transfer
    // DMA1_Channel4->CCR |= DMA_CCR_CIRC;

    // 5. Enable DMA interrupt in NVIC
    NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);  // STM32C0 groups channels 4-7

    // 6. Enable USART2 DMA transmitter
    USART2->CR3 |= USART_CR3_DMAT;

    // 7. Enable DMA1 Channel 4
    DMA1_Channel4->CCR |= DMA_CCR_EN;
}

/**
 * @brief Initialize DMA1 Channel 5 for USART2 RX
 * @param src: Source address (USART2->RDR)
 * @param dst: Destination address (memory buffer)
 * @param len: Number of bytes to receive
 */
void dma1_ch5_uart2_rx_init(uint32_t src, uint32_t dst, uint32_t len)
{
    // 1. Enable clock access to DMA1
    RCC->AHBENR |= AHB_DMA1EN;

    // 2. Disable DMA1 Channel 5
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    while(DMA1_Channel5->CCR & DMA_CCR_EN);

    // 3. Clear all interrupt flags for Channel 5
    DMA1->IFCR |= (0xF << 16);  // Clear all flags for channel 5

    // 4. Configure addresses
    DMA1_Channel5->CPAR = src;  // Peripheral address: USART2->RDR
    DMA1_Channel5->CMAR = dst;  // Memory address: receive buffer
    DMA1_Channel5->CNDTR = len;

    // 5. Configure channel settings
    DMA1_Channel5->CCR = 0;
    DMA1_Channel5->CCR |= DMA_CCR_MINC;      // Memory increment
    DMA1_Channel5->CCR &= ~DMA_CCR_PINC;     // Peripheral no increment
    DMA1_Channel5->CCR &= ~DMA_CCR_DIR;      // Direction: Peripheral to Memory
    DMA1_Channel5->CCR |= DMA_CCR_TCIE;      // Transfer complete interrupt
    DMA1_Channel5->CCR |= DMA_CCR_PL_HIGH;   // High priority
    DMA1_Channel5->CCR |= DMA_CCR_CIRC;      // Circular mode for continuous reception

    // 6. Enable DMA interrupt in NVIC
    NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

    // 7. Enable USART2 DMA receiver
    USART2->CR3 |= (1U << 6);  // DMAR bit

    // 8. Enable DMA1 Channel 5
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

/**
 * @brief DMA interrupt handler for channels 4-7
 * Note: STM32C0 groups channels 4-7 into one IRQ
 */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
    // Check if Channel 4 (USART2 TX) transfer is complete
    if(DMA1->ISR & DMA_ISR_TCIF4)
    {
        // Clear transfer complete flag
        DMA1->IFCR |= DMA_IFCR_CTCIF4;

        // Set flag or handle completion
        dma_tx_complete = 1;

        // Optional: Disable channel after single transfer
        // DMA1_Channel4->CCR &= ~DMA_CCR_EN;
    }

    // Check if Channel 5 (USART2 RX) transfer is complete
    if(DMA1->ISR & (1U << 17))  // TCIF5
    {
        // Clear flag
        DMA1->IFCR |= (1U << 17);  // CTCIF5

        // Handle received data
        // Your RX complete handler here
    }

    // Check for transfer errors on Channel 4
    if(DMA1->ISR & DMA_ISR_TEIF4)
    {
        // Clear error flag
        DMA1->IFCR |= DMA_IFCR_CTEIF4;

        // Handle error
    }
}

/**
 * @brief Send data via USART2 using DMA
 * @param data: Pointer to data buffer
 * @param size: Number of bytes to send
 */
void uart2_dma_send(uint8_t *data, uint16_t size)
{
    // Wait for previous transfer to complete
    while(DMA1_Channel4->CCR & DMA_CCR_EN);

    // Disable channel
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;

    // Clear flags
    DMA1->IFCR |= DMA_IFCR_CGIF4;

    // Set new memory address and length
    DMA1_Channel4->CMAR = (uint32_t)data;
    DMA1_Channel4->CNDTR = size;

    // Enable channel to start transfer
    DMA1_Channel4->CCR |= DMA_CCR_EN;
}

/**
 * @brief Example usage in main
 */
void example_usage(void)
{
    uint8_t tx_buffer[] = "Hello DMA!\r\n";
    uint8_t rx_buffer[100];

    // Initialize USART2 (your existing uart2_tx_init function)
    // uart2_tx_init();

    // Initialize DMA for USART2 TX
    dma1_ch4_uart2_tx_init(
        (uint32_t)tx_buffer,           // Source: memory buffer
        (uint32_t)&USART2->TDR,        // Destination: USART2 TDR register
        sizeof(tx_buffer) - 1          // Length (exclude null terminator)
    );

    // Initialize DMA for USART2 RX (optional)
    dma1_ch5_uart2_rx_init(
        (uint32_t)&USART2->RDR,        // Source: USART2 RDR register
        (uint32_t)rx_buffer,           // Destination: memory buffer
        sizeof(rx_buffer)              // Length
    );

    // Wait for transmission to complete
    while(!dma_tx_complete);

    // Or use the helper function for subsequent transfers
    uart2_dma_send((uint8_t*)"Another message\r\n", 17);
}
