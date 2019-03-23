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

#ifndef __DS3231_H__
#define __DS3231_H__

#include "core.h"

#define DS3231_SLAVER_ADDR		0x68

enum set_type {
	SET_SECOND = 0,
	SET_MINUTES,
	SET_HOUR,
	SET_DAY,
	SET_DATE,
	SET_MONTH,
	SET_YEAR,
};

union timekeeping {
	struct {
		char sec;
		char min;
		char hour;
	} time;
	char times[3];
};

char ds3231_init(void);
char ds3231_enable_oscillator(bool enable);
char ds3231_read_times(union timekeeping *timekeeping);
char ds3231_read_time(enum set_type setting, char *value);
char ds3231_set_times(union timekeeping *timekeeping);
char ds3231_set_time(enum set_type setting, char value);
char ds3231_read_temperature(char *integer, char *decimals);

#endif