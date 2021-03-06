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
 * ANODE_PORT0 <------------+-----------------------+
 *                          |                       |
 *                          |   +-----+             |   +-----+
 *                          +---+  0  |             +---+  2  |
 *                              +--+--+                 +--+--+
 *                                 |                       |
 *                                 |                       |
 *        +------------------------+                       +-------+
 *        |                                                        |
 *        | ANODE_PORT1 <---+-----------------------+              |
 *        |                 |                       |              |
 *        |                 |   +-----+             |   +-----+    |
 *        |                 +---+  1  |             +---+  3  |    |
 *        |                     +--+--+                 +--+--+    |
 *        |                        |                       |       |
 *        |                        |                       |       |
 *        +------------------------+                       +-------+
 *        |                                                        |
 *        v                                                        v
 *  CATHODE_PORT0                                            CATHODE_PORT1
 *
 *
 *  CATHODE_PORT0 <---------+-----------------------+
 *                          |                       |
 *                          |   +-----+             |   +-----+
 *                          +---+  4  |             +---+  6  |
 *                              +--+--+                 +--+--+
 *                                 |                       |
 *                                 |                       |
 *        +------------------------+                       +-------+
 *        |                                                        |
 *        | CATHODE_PORT1 <-+-----------------------+              |
 *        |                 |                       |              |
 *        |                 |   +-----+             |   +-----+    |
 *        |                 +---+  5  |             +---+  7  |    |
 *        |                     +--+--+                 +--+--+    |
 *        |                        |                       |       |
 *        |                        |                       |       |
 *        +------------------------+                       +-------+
 *        |                                                        |
 *        v                                                        v
 *   ANODE_PORT0                                              ANODE_PORT1
 */
#define ANODE_PORT0		P0
#define ANODE_PORT1		P2
#define CATHODE_PORT0		P3
#define CATHODE_PORT1		P4

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

static inline void led_on_delay(unsigned char i)
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

#define DEFINE_MATRIX_DISP(n, anode, cathode)				\
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

#define DEFINE_GPIO_INIT(n, anode, cathode)				\
	/* Reset gpio registers */					\
	static void gpio##n##_init(void)				\
	{								\
		anode		= 0x00;					\
		cathode		= 0x00;					\
		anode##M1 	= 0xff;					\
		anode##M0 	= 0xff;					\
		cathode##M1 	= 0xff;					\
		cathode##M0 	= 0xff;					\
		anode		= 0xff;					\
		cathode		= 0xff;					\
	}

#define DEFINE_MATRIX_DISP_COMBINE(n1, n2, anode, cathode)		\
	DEFINE_GPIO_INIT(n1, anode, cathode)				\
	DEFINE_MATRIX_DISP(n1, anode, cathode);				\
	DEFINE_MATRIX_DISP(n2, cathode, anode)

DEFINE_MATRIX_DISP_COMBINE(0, 4, ANODE_PORT0, CATHODE_PORT0);
DEFINE_MATRIX_DISP_COMBINE(1, 6, ANODE_PORT1, CATHODE_PORT0);
DEFINE_MATRIX_DISP_COMBINE(2, 5, ANODE_PORT0, CATHODE_PORT1);
DEFINE_MATRIX_DISP_COMBINE(3, 7, ANODE_PORT1, CATHODE_PORT1);

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
	void (func_prt_type *show)(struct fb_column_info idata *info);

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
 * fb_scan - Scroll display to dot-matrix screen.
 *
 * @fb_info: The frame buffer info.
 * @n: How many columns to display. This really should range
 *     [MATRIXS_COLUMNS, FB_COLUMNS].
 * @speed: The scroll speed.
 */
int fb_scan(struct fb_info *fb_info, unsigned int n, unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_COLUMNS || n < MATRIXS_COLUMNS)
		return 0;

	for (i = 0; i < n - MATRIXS_MAX_COLUMN; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset++;
	}
	fb_info->offset = offset;

	return n - MATRIXS_COLUMNS;
}

/**
 * fb_scan_reverse - Reverse scroll display to dot-matrix screen.
 *
 * @fb_info: The frame buffer info.
 * @n: How many columns to display. This really should range
 *     [MATRIXS_COLUMNS, FB_COLUMNS].
 * @speed: The scroll speed.
 */
int fb_scan_reverse(struct fb_info *fb_info, unsigned int n,
		    unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_COLUMNS || n < MATRIXS_COLUMNS)
		return 0;

	for (i = 0; i < n - MATRIXS_MAX_COLUMN; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset--;
	}
	fb_info->offset = offset;

	return MATRIXS_COLUMNS - n;
}

/**
 * fb_copy - Copy the buffer from @src to @frame_buffer.
 *
 * @offset: The offset relative to the frame_buffer.
 * @src: The buffer to copy from.
 * @width: The width of the buffer. This is 2 times of the @src buffer size.
 */
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

	gpio0_init();
	gpio1_init();
	gpio2_init();
	gpio3_init();
}
