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

#ifndef __LM75_H__
#define __LM75_H__

#include "core.h"

#define LM75_SLAVER_ADDR		0x48

char lm75_read_temperature(char *integer, char *decimals);

#endif
