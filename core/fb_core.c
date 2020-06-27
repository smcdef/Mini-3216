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
#include <string.h>

/**
 * +---------------+---------------+---------------+---------------+
 * |               |               |               |               |
 * |               |               |               |               |
 * |       0       |       2       |       4       |       6       |
 * |               |               |               |               |
 * |               |               |               |               |
 * +---------------+---------------+---------------+---------------+
 * |               |               |               |               |
 * |               |               |               |               |
 * |       1       |       3       |       5       |       7       |
 * |               |               |               |               |
 * |               |               |               |               |
 * +---------------+---------------+---------------+---------------+
 *
 *        P0 <--------+------------+
 *                    |   +-----+  |   +-----+
 *                    +---+  0  |  +---+  2  |
 *                        +--+--+      +--+--+
 *                           |            |
 *          +----------------+            +-------+
 *          |                                     |
 *          |  P2 <---+------------+              |
 *          |         |   +-----+  |   +-----+    |
 *          |         +---+  1  |  +---+  3  |    |
 *          |             +--+--+      +--+--+    |
 *          |                |            |       |
 *          +----------------+            +-------+
 *          |                                     |
 *          v                                     v
 *          P3                                    P4
 *
 *
 *         P3 <-------+------------+
 *                    |   +-----+  |   +-----+
 *                    +---+  4  |  +---+  6  |
 *                        +--+--+      +--+--+
 *                           |            |
 *          +----------------+            +-------+
 *          |                                     |
 *          |  P4 <---+------------+              |
 *          |         |   +-----+  |   +-----+    |
 *          |         +---+  5  |  +---+  7  |    |
 *          |             +--+--+      +--+--+    |
 *          |                |            |       |
 *          +----------------+            +-------+
 *          |                                     |
 *          v                                     v
 *          P0                                    P2
 */

#if FB_SIZE > 256
#define FB_MEMORY_TYPE		xdata
#elif FB_SIZE > 128
#define FB_MEMORY_TYPE		pdata
#else
#define FB_MEMORY_TYPE		idata
#endif

#define FB_HALF_SIZE_MASK	(sizeof(frame_buffer) / 2 - 1)

static char FB_MEMORY_TYPE frame_buffer[FB_SIZE];

struct fb_column_info {
	char column;
	char byte_h;
	char byte_l;
	unsigned char fair;
	unsigned char brightness;
};

static void led_on_delay(unsigned char i)
{
	while (i--);
}

#define MATRIX_MAX_COLUMN	(MATRIX_COLUMNS - 1)
#define MATRIXS_MAX_COLUMN	(MATRIXS_COLUMNS - 1)

#define LED_ON(mask, anode, brightness)					\
	do {								\
		anode##M1 = ~(mask);					\
		led_on_delay(brightness);				\
		anode##M1 = 0xff;					\
	} while (0)

#define DECLARE_MATRIX_DISP(n, anode, cathode)				\
	static void matrix##n##_disp(char column, char dat,		\
				     unsigned char brightness,		\
				     unsigned char fair)		\
	{								\
		char i;							\
									\
		cathode = ~BIT(column);					\
		for (i = 0; i < MATRIX_COLUMNS; i++) {			\
			char mask = BIT(i);				\
									\
			if (dat & mask) {				\
				LED_ON(mask, anode, brightness);	\
				if (fair > brightness)			\
					led_on_delay(fair - brightness);\
			} else if (fair) {				\
				led_on_delay(fair);			\
			}						\
		}							\
		cathode = 0xff;						\
	}								\
									\
	static void matrix##n##_disp_rotate(char column, char dat, 	\
					    unsigned char brightness,	\
					    unsigned char fair)		\
	{								\
		char i;							\
									\
		cathode = ~BIT(MATRIX_MAX_COLUMN - column);		\
		for (i = 0; i < MATRIX_COLUMNS; i++) {			\
			if (dat & BIT(i)) {				\
				LED_ON(BIT(MATRIX_MAX_COLUMN - i),	\
				       anode, brightness);		\
				if (fair > brightness)			\
					led_on_delay(fair - brightness);\
			} else if (fair) {				\
				led_on_delay(fair);			\
			}						\
		}							\
		cathode = 0xff;						\
	}								\
									\
	/*								\
	 * Just for macro definition ends with a semicolon		\
	 * for Keil C51. Here is just a declaration, so we		\
	 * will not allocate memory for that.				\
	 */								\
	extern char __dummy_##n##_unused__

DECLARE_MATRIX_DISP(0, P0, P3);
DECLARE_MATRIX_DISP(1, P2, P3);
DECLARE_MATRIX_DISP(2, P0, P4);
DECLARE_MATRIX_DISP(3, P2, P4);
DECLARE_MATRIX_DISP(4, P3, P0);
DECLARE_MATRIX_DISP(5, P4, P0);
DECLARE_MATRIX_DISP(6, P3, P2);
DECLARE_MATRIX_DISP(7, P4, P2);

static void fb_show_column(struct fb_column_info idata *info)
{
	char index = info->column >> MATRIX_COLUMN_SHIFT;
	char offset = info->column & MATRIX_COLUMNS_MASK;
	unsigned char brightness = info->brightness;
	unsigned char fair = info->fair;

	switch (index) {
	case 0:
		matrix0_disp(offset, info->byte_h, brightness, fair);
		matrix1_disp(offset, info->byte_l, brightness, fair);
		break;
	case 1:
		matrix2_disp(offset, info->byte_h, brightness, fair);
		matrix3_disp(offset, info->byte_l, brightness, fair);
		break;
	case 2:
		matrix4_disp(offset, info->byte_h, brightness, fair);
		matrix5_disp(offset, info->byte_l, brightness, fair);
		break;
	case 3:
		matrix6_disp(offset, info->byte_h, brightness, fair);
		matrix7_disp(offset, info->byte_l, brightness, fair);
		break;
	default :
		break;
	}
}

static void fb_show_column_rotate(struct fb_column_info idata *info)
{
	char index = info->column >> MATRIX_COLUMN_SHIFT;
	char offset = info->column & MATRIX_COLUMNS_MASK;
	unsigned char brightness = info->brightness;
	unsigned char fair = info->fair;

	switch (index) {
	case 0:
		matrix7_disp_rotate(offset, info->byte_h, brightness, fair);
		matrix6_disp_rotate(offset, info->byte_l, brightness, fair);
		break;
	case 1:
		matrix5_disp_rotate(offset, info->byte_h, brightness, fair);
		matrix4_disp_rotate(offset, info->byte_l, brightness, fair);
		break;
	case 2:
		matrix3_disp_rotate(offset, info->byte_h, brightness, fair);
		matrix2_disp_rotate(offset, info->byte_l, brightness, fair);
		break;
	case 3:
		matrix1_disp_rotate(offset, info->byte_h, brightness, fair);
		matrix0_disp_rotate(offset, info->byte_l, brightness, fair);
		break;
	default :
		break;
	}
}

/**
 * fb_show - Frame buffer show to dot-matrix screen.
 *
 * @fb_info: The frame buffer info.
 */
void fb_show(struct fb_info *fb_info)
{
	unsigned char i;
	struct fb_column_info idata info;
	char FB_MEMORY_TYPE *fb = frame_buffer +
				  ((fb_info->offset & FB_HALF_SIZE_MASK) << 1);
	void (*show)(struct fb_column_info idata *info);

	info.fair = fb_info->fair;
	info.brightness = fb_info->brightness;
	show = fb_info->rotate ? fb_show_column_rotate : fb_show_column;

	for (i = 0; i < MATRIXS_COLUMNS; ++i) {
		info.column = i;
		info.byte_h = *fb++;
		info.byte_l = *fb++;
		if (fb == frame_buffer + sizeof(frame_buffer))
			fb = frame_buffer;
		show(&info);
	}
}

/**
 * @n should range [MATRIXS_COLUMNS, FB_COLUMNS]
 */
unsigned int fb_scan(struct fb_info *fb_info, unsigned int n,
		     unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_COLUMNS || n < MATRIXS_COLUMNS)
		return offset;

	for (i = 0; i < n - MATRIXS_MAX_COLUMN; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset++;
	}
	fb_info->offset = offset;

	return offset + n - MATRIXS_COLUMNS;
}

/**
 * @n should range [MATRIXS_COLUMNS, FB_COLUMNS]
 */
unsigned int fb_scan_reverse(struct fb_info *fb_info, unsigned int n,
			     unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_COLUMNS || n < MATRIXS_COLUMNS)
		return offset;

	for (i = 0; i < n - MATRIXS_MAX_COLUMN; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset--;
	}
	fb_info->offset = offset;

	return offset - (n - MATRIXS_COLUMNS);
}

unsigned char fb_copy(unsigned int offset, const char *src, unsigned char width)
{
	char FB_MEMORY_TYPE *fb = frame_buffer +
				  ((offset & FB_HALF_SIZE_MASK) << 1);
	unsigned char n = width << 1;

	while (n--) {
		*fb++ = *src++;
		if (fb == frame_buffer + sizeof(frame_buffer))
			fb = frame_buffer;
	}

	return width;
}

void fb_init(void)
{
	char FB_MEMORY_TYPE *fb;

	for (fb = frame_buffer; fb < frame_buffer + sizeof(frame_buffer); fb++)
		*fb = 0;

	/* Reset gpio registers */
	P0M1 = 0xff;
	P2M1 = 0xff;
	P3M1 = 0xff;
	P4M1 = 0xff;
	P0M0 = 0xff;
	P2M0 = 0xff;
	P3M0 = 0xff;
	P4M0 = 0xff;
	P0   = 0xff;
	P2   = 0xff;
	P3   = 0xff;
	P4   = 0xff;
}
