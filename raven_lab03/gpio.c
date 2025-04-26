#include "gpio.h"
#include "stm32f446xx.h"

void init_gpio(void)
{
	//enable PORT A for LED output
	RCC->AHB1ENR |= 1<<0;
	
	//Pin 1 output mode and speed for LED 1 
	GPIOA->MODER |= 1<<2;
	GPIOA-> OSPEEDR |= 3<<2;
	
	
	//enable PORT C for button input
	RCC->AHB1ENR |= 1<<2;
	
	//Pin 13 input mode for the mounted button on PORT C Pin 13
	GPIOC->MODER &= ~(3<<26);
	
}
//reading the input from button, default = 1, pressed = 0
int read_pin(void){


	int value = 1;
	if (GPIOC->IDR & 1<<13){
	value = 0;
	}
	return value;
}

//turn led on based on button 
void write_pin(int button)
{
	if (button == 1){
	//if pressed, LED 1 on
	GPIOA->BSRR |= 1<<1;		
	}

	if (button == 0){
	//if not pressed, LED 1 off	
	GPIOA->BSRR |= 1<<1<<16;	
	}
   }
