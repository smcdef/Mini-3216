#ifndef __FB_CORE_H__
#define __FB_CORE_H__

#include "core.h"

#define MATRIX_COLUMN_SHIFT		3
#define MATRIX_COLUMNS			(1 << MATRIX_COLUMN_SHIFT)
#define MATRIX_COLUMNS_MASK		(MATRIX_COLUMNS - 1)

#define MATRIXS_COLUMN_SHIFT		5
#define MATRIXS_COLUMNS			(MATRIX_COLUMNS << 2)
#define MATRIXS_COLUMNS_MASK		(MATRIXS_COLUMNS - 1)

#define FB_SIZE				128
#define FB_COLUMNS			(FB_SIZE >> 1)
#define FB_HALF_COLUMNS			(FB_COLUMNS >> 1)

struct fb_info {
	char offset;
	bool rotate;
	unsigned char fair;
	unsigned char brightness;
};

void fb_off(void);
void fb_show(struct fb_info *fb_info);
void fb_scan(struct fb_info *fb_info, unsigned char n, char speed);
void fb_scan_reverse(struct fb_info *fb_info, unsigned char n, char speed);
unsigned char fb_set(unsigned char offset, const char *src,
		     unsigned char width);
unsigned char fb_clear(unsigned char offset, unsigned char width);

#endif
