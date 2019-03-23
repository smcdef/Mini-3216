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
 * +--------------+---------------+---------------+---------------+
 * |              |               |               |               |
 * |      0       |       2       |       4       |       6       |
 * |              |               |               |               |
 * |              |               |               |               |
 * +--------------+---------------+---------------+---------------+
 * |              |               |               |               |
 * |      1       |       3       |       5       |       7       |
 * |              |               |               |               |
 * |              |               |               |               |
 * +--------------+---------------+---------------+---------------+
 */

#if FB_SIZE > 128
#define MEMORY_TYPE		xdata
#else
#define MEMORY_TYPE		pdata
#endif

#define FB_HALF_SIZE_MASK	(sizeof(frame_buffer) / 2 - 1)

static char MEMORY_TYPE frame_buffer[FB_SIZE];

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

#define DRIVER_ONE_LED(mask, port, brightness)				\
	do {								\
		port |= mask;						\
		port##M1 = ~mask;					\
		port##M0 = mask;					\
		led_on_delay(brightness);				\
		port &= ~mask;						\
	} while(0)

#define DECLARE_MATRIX_DISP(n, port0, port1)				\
	static void matrix##n##_disp(char column, char dat,		\
				     unsigned char brightness,		\
				     unsigned char fair)		\
	{								\
		char i;							\
									\
		port0##M1 &= ~(1 << column);				\
		for (i = 0; i < 8; i++) {				\
			char mask = 1 << i;				\
									\
			if (dat & mask) {				\
				DRIVER_ONE_LED(mask, port1, brightness);\
				if (fair)				\
					led_on_delay(fair - brightness);\
			} else if (fair)				\
				led_on_delay(fair);			\
		}							\
		port0##M1 = 0xff;					\
		port1##M1 = 0xff;					\
		port1##M0 = 0x00;					\
	}								\
									\
	static void matrix##n##_disp_rotate(char column, char dat, 	\
					    unsigned char brightness,	\
					    unsigned char fair)		\
	{								\
		char i;							\
									\
		port0##M1 &= ~(1 << (7 - column));			\
		for (i = 0; i < 8; i++) {				\
			char mask = 1 << i;				\
									\
			if (dat & mask) {				\
				DRIVER_ONE_LED(1 << (7 - i), port1,	\
					       brightness);		\
				if (fair)				\
					led_on_delay(fair - brightness);\
			} else if (fair)				\
				led_on_delay(fair);			\
		}							\
		port0##M1 = 0xff;					\
		port1##M1 = 0xff;					\
		port1##M0 = 0x00;					\
	}								\
									\
/* Just for macro definition ends with a semicolon for Keil C51 */	\
static xdata char __dummy_##n##_unused__

DECLARE_MATRIX_DISP(0, P3, P0);
DECLARE_MATRIX_DISP(1, P3, P2);
DECLARE_MATRIX_DISP(2, P4, P0);
DECLARE_MATRIX_DISP(3, P4, P2);
DECLARE_MATRIX_DISP(4, P0, P3);
DECLARE_MATRIX_DISP(5, P0, P4);
DECLARE_MATRIX_DISP(6, P2, P3);
DECLARE_MATRIX_DISP(7, P2, P4);

void fb_off(void)
{
	P0M1 = P2M1 = P3M1 = P4M1 = 0xff;
	P0M0 = P2M0 = P3M0 = P4M0 = 0x00;
	P0 = P2 = P3 = P4 = 0x00;
}

#ifdef CONFIG_MATRIXS_TEST
#define MATRIXS_FB_SIZE		(MATRIXS_COLUMNS << 1)
void fb_matrixs_test(void)
{
	unsigned char i, j, k;
	char MEMORY_TYPE *fb = frame_buffer;
	struct fb_info fb_info;

	memset(&fb_info, 0, sizeof(fb_info));
	fb_set(0, 0, 32);
	fb_info.brightness = 50;

	for (i = 0; i < MATRIXS_FB_SIZE; ++i, ++fb) {
		for (j = 0; j < MATRIX_COLUMNS; ++j) {
			*fb |= 1 << j;
			for (k = 0; k < ((MATRIXS_FB_SIZE - i) >> 2) + 5; ++k)
				fb_show(&fb_info);
		}
	}

	for (i = 0; i < 50; ++i)
		fb_show(&fb_info);
}
#else
void fb_matrixs_test(void)
{
}
#endif

static void fb_show_column(struct fb_column_info pdata *fb_column_info)
{
	char index = fb_column_info->column >> MATRIX_COLUMN_SHIFT;
	char offset = fb_column_info->column & MATRIX_COLUMNS_MASK;
	unsigned char brightness = fb_column_info->brightness;
	unsigned char fair = fb_column_info->fair;

	switch(index) {
	case 0:
		matrix0_disp(offset, fb_column_info->byte_h, brightness, fair);
		matrix1_disp(offset, fb_column_info->byte_l, brightness, fair);
		break;
	case 1:
		matrix2_disp(offset, fb_column_info->byte_h, brightness, fair);
		matrix3_disp(offset, fb_column_info->byte_l, brightness, fair);
		break;
	case 2:
		matrix4_disp(offset, fb_column_info->byte_h, brightness, fair);
		matrix5_disp(offset, fb_column_info->byte_l, brightness, fair);
		break;
	case 3:
		matrix6_disp(offset, fb_column_info->byte_h, brightness, fair);
		matrix7_disp(offset, fb_column_info->byte_l, brightness, fair);
		break;
	default :
		break;
	}
}

static void fb_show_column_rotate(struct fb_column_info pdata *fb_column_info)
{
	char index = fb_column_info->column >> MATRIX_COLUMN_SHIFT;
	char offset = fb_column_info->column & MATRIX_COLUMNS_MASK;
	unsigned char brightness = fb_column_info->brightness;
	unsigned char fair = fb_column_info->fair;

	switch(index) {
	case 0:
		matrix7_disp_rotate(offset, fb_column_info->byte_h,
				    brightness, fair);
		matrix6_disp_rotate(offset, fb_column_info->byte_l,
				    brightness, fair);
		break;
	case 1:
		matrix5_disp_rotate(offset, fb_column_info->byte_h,
				    brightness, fair);
		matrix4_disp_rotate(offset, fb_column_info->byte_l,
				    brightness, fair);
		break;
	case 2:
		matrix3_disp_rotate(offset, fb_column_info->byte_h,
				    brightness, fair);
		matrix2_disp_rotate(offset, fb_column_info->byte_l,
				    brightness, fair);
		break;
	case 3:
		matrix1_disp_rotate(offset, fb_column_info->byte_h,
				    brightness, fair);
		matrix0_disp_rotate(offset, fb_column_info->byte_l,
				    brightness, fair);
		break;
	default :
		break;
	}
}

/**
 * fb_show - Frame buffer show to dot-matrix screen.
 *
 * @offset: The offset relative to the frame_buffer
 * @brightness: The brightness of each LED
 * @rotate: Whether to rotate display
 * @fair: If @fair is true, each LED will be on or off for the same time.
 *	  Otherwise, only the LED that needs to be on will delay.
 */
void fb_show(struct fb_info *fb_info)
{
	unsigned char i;
	struct fb_column_info pdata fb_column_info;
	char MEMORY_TYPE *fb = frame_buffer;
	void (code *show)(struct fb_column_info pdata *fb_column_info);

	fb_info->offset &= FB_HALF_SIZE_MASK;
	fb_column_info.fair = fb_info->fair;
	fb_column_info.brightness = fb_info->brightness;
	fb += fb_info->offset << 1;

	show = fb_info->rotate ? fb_show_column_rotate : fb_show_column;
	for (i = 0; i < MATRIXS_COLUMNS; ++i) {
		fb_column_info.column = i;
		fb_column_info.byte_h = *fb++;
		fb_column_info.byte_l = *fb++;
		if (fb == frame_buffer + sizeof(frame_buffer))
			fb = frame_buffer;
		show(&fb_column_info);
	}
}

/**
 * @n should range [32, FB_SIZE / 2]
 */
unsigned int fb_scan(struct fb_info *fb_info, unsigned int n,
		     unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_SIZE / 2 || n < 32)
		return offset;

	for (i = 0; i < n - MATRIXS_COLUMNS_MASK; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset++;
	}
	fb_info->offset = offset;

	return offset + n - MATRIXS_COLUMNS;
}

/**
 * @n should range [32, FB_SIZE / 2]
 */
unsigned int fb_scan_reverse(struct fb_info *fb_info, unsigned int n,
			     unsigned char speed)
{
	unsigned int i, offset = fb_info->offset;
	unsigned char j;

	if (n > FB_SIZE / 2 || n < 32)
		return offset;

	for (i = 0; i < n - MATRIXS_COLUMNS_MASK; ++i) {
		for (j = 0; j < speed; ++j)
			fb_show(fb_info);
		fb_info->offset--;
	}
	fb_info->offset = offset;

	return offset - (n - MATRIXS_COLUMNS);
}

/**
 * @offset + @width should range [0, FB_SIZE / 2].
 */
unsigned int fb_copy(unsigned int offset, const char *src, unsigned int width)
{
	char MEMORY_TYPE *fb = frame_buffer;
	unsigned int ret = width;

	offset &= FB_HALF_SIZE_MASK;
	fb += offset << 1;

	width <<= 1;
	while (width--) {
		*fb++ = *src++;
		if (fb == frame_buffer + sizeof(frame_buffer))
			fb = frame_buffer;
	}

	return ret;
}

unsigned int fb_set(unsigned int offset, char c, unsigned int width)
{
	unsigned int ret = width;
	char arr[2];

	arr[0] = c;
	arr[1] = c;
	while (width--)
		offset += fb_copy(offset, arr, 1);

	return ret;
}
