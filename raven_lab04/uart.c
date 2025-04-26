#include "stm32f446xx.h"
#include "uart.h"

void UartInit()
{
    // Enabling Clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configuring RX and TX Pin
    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;
    GPIOA->OTYPER |= GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3;
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos;

    // Enabling Clock for UART
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Disabling UART for Configuring
    USART2->CR1 &= ~USART_CR1_UE;

    // Enabling TX
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // Configuring Baud Rate
    uint32_t uart_div = /*GetAPB1Clock()*/ 16000000UL / 115200U;

    USART2->BRR = ((uart_div / 16) << USART_BRR_DIV_Mantissa_Pos) | ((uart_div % 16) << USART_BRR_DIV_Fraction_Pos);

    // Enabling UART
    USART2->CR1 |= USART_CR1_UE;
}


void UartSendBuffer(char *buffer)
{
    while (*buffer)
    {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = *buffer++;
    }
}