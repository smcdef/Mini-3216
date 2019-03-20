#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_FB_SIZE_SHIFT		10
/**
 * CONFIG_FB_SIZE must be 2 to the power of n. Also, it should greater than or
 * equal to 128. For example 128, 256, 512, 1024 or 2048...
 */
#define CONFIG_FB_SIZE			(1 << CONFIG_FB_SIZE_SHIFT)
#define CONFIG_MAIN_FOSC                27000000L /* system main clock */
/* #define CONFIG_DS3231_INT */
/* #define CONFIG_BS813A */

#if CONFIG_FB_SIZE_SHIFT < 7
#error "CONFIG_FB_SIZE_SHIFT should greater than or equal to 7"
#endif

#endif
