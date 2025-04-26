#include "stm32f446xx.h"
#include <stdbool.h>
#include "uart.h"

#define RX_BUFFER_SIZE 20

uint8_t rxBuffer[RX_BUFFER_SIZE];

void SPI_GPIOInit()
{
    RCC->AHB1ENR |= (1 << 0); // Enable GPIOA Clock

    GPIOA->MODER |= (2 << 10) | (2 << 12) | (2 << 14); // Set PA5, PA6, PA7 to Alternate Function mode (SCK, MISO, MOSI)
    GPIOA->MODER &= ~(3U << 8);                        // Set PA4 (NSS) to input mode
    GPIOA->PUPDR |= 1 << 8;

    GPIOA->OSPEEDR |= (3 << 10) | (3 << 12) | (3 << 14); // Set PA5, PA6, PA7 to high speed

    GPIOA->AFR[0] |= (5 << (4 * 5)) | (5 << (4 * 6)) | (5 << (4 * 7)); // Set PA5, PA6, PA7 to AF5 (SPI1)
}

void SPI_Config(void)
{
    RCC->APB2ENR |= (1 << 12); // Enable SPI1 Clock

    SPI1->CR1 &= ~(1 << 6); // Disable SPI1

    SPI1->CR1 &= ~(1 << 2); // Clear MSTR bit -> Slave mode

    SPI1->CR1 &= ~(1 << 0 | 1 << 1); // CPOL = 0, CPHA = 0

    SPI1->CR1 &= ~(1 << 7); // LSBFIRST = 0, MSB first

    SPI1->CR1 &= ~(1 << 8); // Clear SSI (Slave Select internal)

    SPI1->CR1 &= ~(1 << 9); // SSM = 0, hardware slave management

    SPI1->CR1 &= ~(1 << 10); // RXONLY = 0, full-duplex

    SPI1->CR1 &= ~(1 << 11); // DFF=0, 8 bit data

    SPI1->CR2 = 0;

    SPI1->CR1 |= (1 << 6); // Enable SPI1
}

void SPI_Receive(uint8_t *buffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (!(SPI1->SR & SPI_SR_RXNE))
            ;                 // Wait until RXNE is set
        buffer[i] = SPI1->DR; // Read the data from the SPI data register
        if (buffer[i] == '\n')
        {
            buffer[i + 1] = '\0';
            break;
        }
    }
}

int main(void)
{
    SPI_GPIOInit();
    SPI_Config();
    UartInit();

    while (1)
    {
        while ((GPIOA->IDR & (1 << 4)))
            ;
        SPI_Receive(rxBuffer, sizeof(rxBuffer));

        while (!(GPIOA->IDR & (1 << 9)))
            ;
        UartSendBuffer("Data Received Successfully.\n");
        GPIOA->BSRR |= 1 << 21;
    }
}