#ifndef INC_UART_H
#define INC_UART_H
#include "stm32f446xx.h"
#include "stm32f446xx.h"
#include <stdio.h>

void UartInit();
void UART_SendChar(uint8_t ch);
uint8_t UART_ReceiveChar();
void UartSendBuffer(char buffer[]);
void UartReceiveBuffer(char buffer[], uint8_t len);

#endif

