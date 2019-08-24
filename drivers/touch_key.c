/**
 * Internal touch-key API.
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

#include "touch_key.h"
#include "delay.h"

#ifdef CONFIG_BS813A
#define key1 P5_4
#define key2 P5_5
#define key3 P1_2

char touch_key_read(char *key_value)
{
	char count = 0, value = 0;

	if (key1 == 0) {
		count++;
		value |= KEY_ENTER;
	}
	if (key2 == 0) {
		count++;
		value |= KEY_RIGHT;
	}
	if (key3 == 0) {
		count++;
		value |= KEY_LEFT;
	}

	if (key_value)
		*key_value = value;

	return count;
}

void touch_key_init(void)
{
}
#else
#define unused P1_2
#define Data   P5_5
#define Clock  P5_4

static char key_read_byte(void)
{
	char i;
	unsigned char dat = 0;

	for(i = 0; i < 8; i++) {
		Clock = 0;
		udelay(10);
		dat >>= 1;
		Clock = 1;
		if(Data)
			dat |= 0x80;
		udelay(10);
	}

	return dat;
}

char touch_key_read(char *key_value)
{
	char dat = key_read_byte();
	char value;

	value = ~dat & (KEY_ENTER | KEY_LEFT | KEY_RIGHT);
	if (key_value)
		*key_value = value;

	return (dat >> 4) & GENMASK(2, 0);
}

void touch_key_init(void)
{
	unused = 0;
}
#endif /* CONFIG_BS813A */
