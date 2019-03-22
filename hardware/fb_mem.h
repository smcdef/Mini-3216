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

#ifndef __FB_MEM_H__
#define __FB_MEM_H__

#include "core.h"

unsigned int fb_scan_string(struct fb_info *fb_info, unsigned char speed,
                            const char *s);
unsigned int fb_copy_string(unsigned int offset, const char *s);

#endif
