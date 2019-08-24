/**
 * Internal delay functions..
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
		NOP();
		i = 4;
		while (--i);
	}
}
