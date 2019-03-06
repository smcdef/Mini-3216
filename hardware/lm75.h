#ifndef __LM75_H__
#define __LM75_H__

#include "core.h"

#define LM75_SLAVER_ADDR		0x48

char lm75_read_temperature(char *integer, char *decimals);

#endif