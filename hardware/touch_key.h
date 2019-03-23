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

#ifndef _TOUCH_KEY_H__
#define _TOUCH_KEY_H__

#include "core.h"

#define KEY_ENTER		BIT(0)
#define KEY_RIGHT		BIT(1)
#define KEY_LEFT		BIT(2)

char touch_key_read(char *key_value);
void touch_key_init(void);

#endif
