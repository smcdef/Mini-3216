#ifndef __CONFIG_H
#define __CONFIG_H

/**
 * CONFIG_FB_SIZE must be 2 to the power of n. Also, it should greater than or
 * equal to 128. For example 128, 256, 512, 1024 or 2048...
 */
#define CONFIG_FB_SIZE			1024
#define CONFIG_MAIN_FOSC                27000000L /* system main clock */
/* #define CONFIG_DS3231_INT */
/* #define CONFIG_BS813A */

#endif
