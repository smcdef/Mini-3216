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

#include "fb_core.h"
#include "font.h"
#include "fb_mem.h"

unsigned int fb_scan_string(struct fb_info *fb_info, unsigned int offset,
			    unsigned char speed, const char *s)
{
	char code *encode;
	unsigned int new_columns = offset;

	offset += fb_info->offset;

	while (*s) {
		signed char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += (char)encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
		new_columns += width;
		if (new_columns > FB_COLUMNS - CHARACTER_WIDTH) {
			fb_info->offset += fb_scan(fb_info, new_columns - 1,
						   speed) + 1;
			new_columns = MATRIXS_COLUMNS;
		}
	}

	return fb_scan(fb_info, new_columns, speed);
}

unsigned int fb_copy_string(unsigned int offset, const char *s)
{
	char code *encode;
	unsigned int offset_old = offset;

	while (*s) {
		signed char width = search_encode(s, &encode);

		/* skip invalid input */
		if (width < 0) {
			s += (char)encode;
			continue;
		}
		s += width == CHARACTER_WIDTH ? ENCODE_INDEX_SIZE : 1;
		offset += fb_copy(offset, encode, width);
	}

	return offset - offset_old;
}
