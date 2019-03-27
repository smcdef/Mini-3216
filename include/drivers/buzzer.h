/**
 * Author:   smcdef <smcdef@163.com>
 * Date:     2017-05-08
 * Describe: buzzer driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "core.h"

void buzzer_power_on(void);
void buzzer_enter(void);
void buzzer_key(void);
void buzzer_chime(void);

#endif
