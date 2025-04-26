
#include "gpio.h"
int main()
{
	init_gpio();
	while(1)
	{
		write_pin(read_pin());
		for(int i = 0; i<100; i++);
	}
	
}
