
#ifndef __GPIO_H
#define __GPIO_H
#include "stdint.h"
void init_gpio(void);
void write_pin(int);
int read_pin(void);
#endif
