/**
 * Internal ADC functions.
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

#ifndef _ADC_H__
#define _ADC_H__

#include "core.h"

#define ADC_FLAG                0x10

void adc_init(unsigned char channel);
void adc_start(unsigned char channel);
unsigned char adc_read(unsigned char channel);

#endif
