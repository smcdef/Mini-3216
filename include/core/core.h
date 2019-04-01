/**
 * Core macro definition and others.
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
#ifndef __CORE_H__
#define __CORE_H__

#include "stc15f2k60s2.h"
#include "config.h"

enum {
	false = 0,
	true = 1,
};

typedef unsigned char bool;

#ifndef NULL
#define NULL	((void *)0)
#endif

/**
 * ARRAY_SIZE - get the number of elements in array @arr
 * @arr: array to be sized
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* align @num on a @align boundary - adjust @num up/down if needed */
#define ALIGN_DOWN(num, align)  ((num) & (~((align) - 1)))
#define ALIGN_UP(num, align)    ALIGN_DOWN(num + align - 1, align)

#define BIT(nr)			(1U << (nr))

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK(4, 1) gives us the 16bit vector 0x001e.
 */
#define BITS_PER_INT		(sizeof(int) << 3)
#define GENMASK(h, l) \
	(((~0U) - (1U << (l)) + 1) & (~0U >> (BITS_PER_INT - 1 - (h))))

#define DEC_TO_BCD(dec)		((dec) / 10 * 16 + (dec) % 10)
#define BCD_TO_DEC(bcd)		((bcd) / 16 * 10 + (bcd) % 16)

#endif
