/**
 * Internal uart API.
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

#ifndef __USART_H__
#define __USART_H__

#include "core.h"

void uart_init(void);
void uart_putc(char dat);
void uart_puts(char *str);

#endif
