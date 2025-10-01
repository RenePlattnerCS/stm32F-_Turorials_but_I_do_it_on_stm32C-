#include <stdint.h>
#include <stdio.h>
#include "stm32C0xx.h"
#include "uart.h"
#include "timer.h"

#include "systick.h"


#include "stm32c0xx.h"

#define GPIOAEN     (1U<<0)
#define GPIOCEN     (1U<<2)
#define SYSCFGEN    (1U<<0)

void pc13_exti_init2(void);
void led_init(void);
void delay(volatile uint32_t s);

int main(void)
{
    led_init();
    pc13_exti_init2();

    while (1)
    {
        // Main loop does nothing; LED blink is handled in the interrupt
    }
}

void led_init(void)
{
    // Enable GPIOA clock
    RCC->IOPENR |= GPIOAEN;

    // Set PA5 as output
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));
}

void pc13_exti_init2(void)
{
    // Enable GPIOC clock
    RCC->IOPENR |= (1U<<2);

    // PC13 as input
    GPIOC->MODER &= ~(3U << (13 * 2));

    // Enable SYSCFG clock
    RCC->APBENR2 |= (1U<<0);

    // Map EXTI13 to Port C
    EXTI->EXTICR[3] &= ~(0xFU << 4);
    EXTI->EXTICR[3] |=  (0x2U << 4);  // Port C = 0b0010

    // Configure EXTI13
    EXTI->IMR1  |= (1U << 13);   // Unmask interrupt
    EXTI->RTSR1 &= ~(1U << 13);  // Disable rising edge
    EXTI->FTSR1 |=  (1U << 13);  // Enable falling edge

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

// Simple blocking delay (not precise, but enough for LED blink demo)
void delay(volatile uint32_t s)
{
    for (; s>0; s--);
}

void EXTI4_15_IRQHandler(void)
{
    if (EXTI->RPR1 & (1U << 13))   // Check if EXTI13 caused interrupt
    {
        EXTI->RPR1 = (1U << 13);   // Clear pending flag

        // Blink LED 3 times
        for (int i = 0; i < 3; i++)
        {
            GPIOA->ODR ^= (1U << 5);  // Toggle LED
            delay(100000);            // crude delay
            GPIOA->ODR ^= (1U << 5);  // Toggle LED
            delay(100000);
        }
    }
}
