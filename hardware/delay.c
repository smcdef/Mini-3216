#include "delay.h"

void mdelay(unsigned int ms)
{
	unsigned int i;

	do {
		i = CONFIG_MAIN_FOSC / 13000;
		while (--i);
	} while (--ms);
}

void udelay(unsigned int n)
{
	unsigned char i;

	while (n--) {
		_nop_();
		i = 4;
		while (--i);
	}
}
