/**
 * Internal frame buffer API.
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

#ifndef __FB_CORE_H__
#define __FB_CORE_H__

#include "core.h"

#define MATRIX_COLUMN_SHIFT		3
#define MATRIX_COLUMNS			(1 << MATRIX_COLUMN_SHIFT)
#define MATRIX_COLUMNS_MASK		(MATRIX_COLUMNS - 1)

#define MATRIXS_COLUMN_SHIFT		5
#define MATRIXS_COLUMNS			(MATRIX_COLUMNS << 2)
#define MATRIXS_COLUMNS_MASK		(MATRIXS_COLUMNS - 1)

/**
 * FB_SIZE must be 2 to the power of n. Also, it should greater than or
 * equal to 128. For example 128, 256, 512, 1024 or 2048...
 */
#define FB_SIZE				CONFIG_FB_SIZE
#define FB_COLUMNS			(FB_SIZE >> 1)

struct fb_info {
	unsigned int offset;
	bool rotate;
	unsigned char fair;
	unsigned char brightness;
};

void fb_off(void);
void fb_show(struct fb_info *fb_info);
unsigned int fb_scan(struct fb_info *fb_info, unsigned int n,
		     unsigned char speed);
unsigned int fb_scan_reverse(struct fb_info *fb_info, unsigned int n,
			     unsigned char speed);
unsigned int fb_copy(unsigned int offset, const char *src,
		     unsigned int width);
unsigned int fb_set(unsigned int offset, char c, unsigned int width);

#endif
