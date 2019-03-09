#include "usart.h"

/* 9600bps@27.000MHz */
void uart_init(void)
{
	SCON  = 0x50;
	AUXR |= 0x40;
	AUXR &= 0xFE;
	TMOD &= 0x0F;
	TL1   = 0x41;
	TH1   = 0xFD;
	ET1   = 0;
	TR1   = 1;
	ES    = 0;
}

void uart_putc(char dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}

void uart_puts(char *str)
{
	while (*str)
		uart_putc(*str++);
}
