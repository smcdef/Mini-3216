#ifndef _TOUCH_KEY_H__
#define _TOUCH_KEY_H__

#include "core.h"

#define KEY_ENTER		BIT(0)
#define KEY_RIGHT		BIT(1)
#define KEY_LEFT		BIT(2)

char touch_key_read(char *key_value);

#endif
