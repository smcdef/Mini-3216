/**
 * All configuration options.
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

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_FB_SIZE_SHIFT			7
/**
 * CONFIG_FB_SIZE must be 2 to the power of n. Also, it should greater than or
 * equal to 128. For example 128, 256, 512, 1024 or 2048...
 */
#define CONFIG_FB_SIZE				(1 << CONFIG_FB_SIZE_SHIFT)
/* system main clock */
#define CONFIG_MAIN_FOSC			27000000L
#define CONFIG_BS813A
#define CONFIG_PRES_ADC_CHANNEL			6
#define CONFIG_MAX_BRIGHTNESS			80
#define CONFIG_NIGHT_MODE_BRIGHTNESS		1
#define CONFIG_NIGHT_MODE_FAIR_FACTOR		55
#define CONFIG_SCAN_SPEED_BRIGHTNESS		55
#define CONFIG_SEETING_TIME_SCAN_SPEED		15
#define CONFIG_SETTING_SCAN_SPEED		2
#define CONFIG_CHIME_START_HOUR			8
#define CONFIG_CHIME_END_HOUR			22

#if CONFIG_FB_SIZE_SHIFT < 7
#error "CONFIG_FB_SIZE_SHIFT should greater than or equal to 7"
#endif

#if CONFIG_PRES_ADC_CHANNEL > 7 || CONFIG_PRES_ADC_CHANNEL < 0
#error "CONFIG_PRES_ADC_CHANNEL should be less than or equal to 0 or \
	greater than or equal to 7"
#endif

#if defined (SDCC) || defined (__SDCC)
#define xdata		__xdata
#define pdata		__pdata
#define idata		__idata
#define code		__code
#define bit		__bit
#define interrupt	__interrupt
#define using		__using
#define func_prt_type
#else
#define func_prt_type	code
#define inline
#endif

#endif
