/**
 * Internal uart API.
 *
 * Copyright (c) 2019-2020  smcdef.
 *
 * Author: songmuchun <smcdef@163.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "usart.h"

/* 9600bps@27.000MHz */
void uart_init(void)
{
	SCON  = 0x50;
	AUXR |= 0x01;
	AUXR |= 0x04;
	T2L   = 0x41;
	T2H   = 0xFD;
	AUXR |= 0x10;
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
