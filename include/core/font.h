/**
 * Internal font API.
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

#ifndef __FONT_H__
#define __FONT_H__

#include "core.h"

#define NONE_ASCII		'\x7f'

#define CHARACTER_WIDTH		16
#define ASCII_WIDTH		6

#ifdef GB2312_ENCODE
#define ENCODE_INDEX_SIZE	2
#else
#define ENCODE_INDEX_SIZE	3
#endif

signed char search_encode(const char *index, char code **out);
void font_sort(void);

#endif
