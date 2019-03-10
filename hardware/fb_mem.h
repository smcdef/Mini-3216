#ifndef __FB_MEM_H__
#define __FB_MEM_H__

#include "core.h"

unsigned char fb_scan_string(struct fb_info *fb_info, char speed,
			     const char *s);
unsigned char fb_set_string(unsigned char offset, const char *s);

#endif
