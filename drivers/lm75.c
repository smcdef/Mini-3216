/**
 * Internal lm75 API.
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

#include "lm75.h"
#include "i2c.h"

#define TEMP_MSB_REGISTER_ADDR		0x00
#define TEMP_LSB_REGISTER_ADDR		0x01

char lm75_read_temperature(char *integer, char *decimals)
{
	char temp[2];

	if (i2c_read(LM75_SLAVER_ADDR, TEMP_MSB_REGISTER_ADDR, temp,
		     sizeof(temp)))
		return -1;

	*decimals = 0;
	if (temp[1] & BIT(6))
		*decimals += 25;
	if (temp[1] & BIT(7))
		*decimals += 50;
	*integer = temp[0];

	return 0;
}
