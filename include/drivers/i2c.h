/**
 * Internal i2c API.
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

#ifndef __I2C_H__
#define __I2C_H__

#include "core.h"

char i2c_transfer(char slave_addr, char addr, void *buf, unsigned char len);
char i2c_read(char slave_addr, char addr, void *buf, unsigned char len);
void i2c_init(void);

#endif