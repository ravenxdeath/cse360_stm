#include "stm32f446xx.h"
void TimerInit(void)
{
    /* Enable Clock for Timer6 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    /* Reset Timer6 configuration */
    TIM6->CR1 = 0;
    TIM6->PSC = 90 - 1; // Prescaler to get 1 MHz frequency
    TIM6->ARR = 0xFFFF; // Auto-reload register max value

    TIM6->EGR |= TIM_EGR_UG;

    /* Enable Timer6 */
    TIM6->CR1 |= TIM_CR1_CEN;

    while (!(TIM6->SR & TIM_SR_UIF))
        ;
}

/**
 * @brief Delays for a specified number of microseconds
 * @param delay: number of microseconds to delay
 */
void DelayUS(uint16_t delay)
{
    TIM6->CNT = 0; // Reset the counter
    while (TIM6->CNT < delay)
        ; // Wait until the counter reaches the delay value
}

/**
 * @brief Delays for a specified number of milliseconds
 * @param delay: number of milliseconds to delay
 */
void DelayMS(uint16_t delay)
{
    while (delay--)
    {
        DelayUS(1000); // Delay for 1000 microseconds (1 millisecond)
    }
}


void CS_E(void)
{
    GPIOA->BSRR |= (1 << 9) << 16;
}

void CS_D(void)
{
    GPIOA->BSRR |= (1 << 9);
}

void SPI_GPIOInit()
{
    RCC->AHB1ENR |= (1 << 0); // Enable GPIOA Clock

    GPIOA->MODER |= (2 << 10) | (2 << 12) | (2 << 14) | (1 << 18); // Set PA4, PA5, PA6, PA7 to Alternate Function mode

    GPIOA->OSPEEDR |= (3 << 10) | (3 << 12) | (3 << 14); // Set PA4, PA5, PA6, PA7 to high speed

    GPIOA->AFR[0] |= (5 << (4 * 5)) | (5 << (4 * 6)) | (5 << (4 * 7)); // Set PA4, PA5, PA6, PA7 to AF5 (SPI1)

    // Peripheral Clock / 2
    // APB 90MHz = 45MHz
}

void SPI_Config(void)
{
    // PA5 ---> D13
    // PA6 ---> D12
    // PA7 ---> D11
    // PA9 ---> D10
    // GND ---> GND Note: this is only needed if we use two different computers

    RCC->APB2ENR |= (1 << 12); // Enable SPI1 CLock

    SPI1->CR1 &= ~(1 << 6); // Disabling SPI1

    SPI1->CR1 &= ~(1 << 0 | 1 << 1); // CPOL = 0, CPHA = 0

    SPI1->CR1 |= (1 << 2); // Master Mode

    SPI1->CR1 |= 7U << 3; // BR[2:0] = 011: fPCLK/16, PCLK2 = 90MHz, SPI clk = 5MHz

    SPI1->CR1 &= ~(1 << 7); // LSBFIRST = 0, MSB first

    SPI1->CR1 |= (1 << 9) | (1 << 8); // SSM=1, SSI=1 -> Software Slave Management

    SPI1->CR1 &= ~(1 << 10); // RXONLY = 0, full-duplex

    SPI1->CR1 &= ~(1 << 11); // DFF=0, 8 bit data

    SPI1->CR2 = 0;

    SPI1->CR1 |= (1 << 6);
}

void SPI_Transmit(uint8_t *data, int size)
{

    /************** STEPS TO FOLLOW *****************
    1. Wait for the TXE bit to set in the Status Register
    2. Write the data to the Data Register
    3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
    4. Clear the Overrun flag by reading DR and SR
    ************************************************/

    int i = 0;
    while (i < size)
    {
        while (!((SPI1->SR) & (1 << 1)))
        {
        }; // wait for TXE bit to set -> This will indicate that the buffer is empty
        SPI1->DR = data[i]; // load the data into the Data Register
        i++;
    }

    /*During discontinuous communications, there is a 2 APB clock period delay between the
    write operation to the SPI_DR register and BSY bit setting. As a consequence it is
    mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
    data.
    */
    while (((SPI1->SR) & (1 << 7)))
    {
    }; // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
}

int main()
{
    TimerInit();
    SPI_GPIOInit();
    SPI_Config();

    uint8_t buffer[] = "Hello Arduino It's STM32!\n";

    while (1)
    {
        CS_E(); // NSS
        SPI_Transmit(buffer, sizeof(buffer));
        CS_D(); // NSS
        DelayMS(100);
    }
}