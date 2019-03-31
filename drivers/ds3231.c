/**
 * Internal ds3231 API.
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

#include "ds3231.h"
#include "i2c.h"

#define SECONDS_REGISTER_ADDR		0x00
#define MINUTES_REGISTER_ADDR		0x01
#define HOUR_REGISTER_ADDR		0x02
#define DAY_REGISTER_ADDR		0x03
#define DATE_REGISTER_ADDR		0x04
#define MONTH_REGISTER_ADDR		0x05
#define YEAR_REGISTER_ADDR		0x06

#define CONTROL_REGISTER_ADDR		0x0e
#define STATUS_REGISTER_ADDR		0x0f
#define AGING_OFFSET_REGISTER_ADDR	0x10
#define TEMP_MSB_REGISTER_ADDR		0x11
#define TEMP_LSB_REGISTER_ADDR		0x12

char ds3231_init(void)
{
	char value;

	if (i2c_read(DS3231_SLAVER_ADDR, STATUS_REGISTER_ADDR, &value, 1))
		return -1;
	if (value & BIT(7)) {
		char times[] = { 0x56, 0x34, 0x12, 0x02, 0x01, 0x01, 0x19 };

		if (i2c_transfer(DS3231_SLAVER_ADDR, SECONDS_REGISTER_ADDR,
				 times, sizeof(times)))
			return -1;
		value &= ~BIT(7);
	}

	value &= ~BIT(3);
	if (i2c_transfer(DS3231_SLAVER_ADDR, STATUS_REGISTER_ADDR, &value, 1))
		return -1;

	if (i2c_read(DS3231_SLAVER_ADDR, CONTROL_REGISTER_ADDR, &value, 1))
		return -1;
	value &= ~(BIT(2) | BIT(3) | BIT(4));

	return i2c_transfer(DS3231_SLAVER_ADDR, CONTROL_REGISTER_ADDR,
			    &value, 1);
}

char ds3231_enable_oscillator(bool enable)
{
	char value;

	if (i2c_read(DS3231_SLAVER_ADDR, CONTROL_REGISTER_ADDR, &value, 1))
		return -1;

	enable = !enable;
	if (!!(value & BIT(7)) == !!enable)
		return 0;

	if (enable)
		value |= BIT(7);
	else
		value &= ~BIT(7);

	return i2c_transfer(DS3231_SLAVER_ADDR, CONTROL_REGISTER_ADDR,
			    &value, 1);
}

char ds3231_read_date(struct rtc *rtc)
{
	return i2c_read(DS3231_SLAVER_ADDR, SECONDS_REGISTER_ADDR, &rtc->day, 4);
}

char ds3231_read_times(struct rtc *rtc)
{
	return i2c_read(DS3231_SLAVER_ADDR, SECONDS_REGISTER_ADDR, rtc, 3);
}

char ds3231_read_time(enum set_type setting, char *value)
{
	return i2c_read(DS3231_SLAVER_ADDR, (char)setting, value, 1);
}

char ds3231_set_date(struct rtc *rtc)
{
	return i2c_transfer(DS3231_SLAVER_ADDR, SECONDS_REGISTER_ADDR,
			    &rtc->day, 4);
}

char ds3231_set_times(struct rtc *rtc)
{
	return i2c_transfer(DS3231_SLAVER_ADDR, SECONDS_REGISTER_ADDR, rtc, 3);
}

char ds3231_set_time(enum set_type setting, char value)
{
	return i2c_transfer(DS3231_SLAVER_ADDR, (char)setting, &value, 1);
}

char ds3231_read_temperature(char *integer, char *decimals)
{
	char temp[2];

	if (i2c_read(DS3231_SLAVER_ADDR, TEMP_MSB_REGISTER_ADDR, temp,
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