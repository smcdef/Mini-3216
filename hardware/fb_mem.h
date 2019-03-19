#ifndef __FB_MEM_H__
#define __FB_MEM_H__

#include "core.h"

unsigned int fb_scan_string(struct fb_info *fb_info, char speed, const char *s);
unsigned int fb_set_string(unsigned int offset, const char *s);

#endif
