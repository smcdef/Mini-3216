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

#include "i2c.h"
#include "delay.h"

sbit scl = P1 ^ 4;
sbit sda = P1 ^ 5;

#define NOP(n)					\
	do {					\
		unsigned char i;		\
						\
		for (i = 0; i < n; ++i)		\
			_nop_();		\
	} while (0)

static void i2c_start(void)
{
	sda = 1;
	NOP(2);
	scl = 1;
	udelay(5);
	sda = 0;
	udelay(5);
	scl = 0;
	NOP(3);
}

static void i2c_stop(void)
{
	sda = 0;
	_nop_();
	scl = 1;
	udelay(5);
	sda = 1;
	NOP(5);
}

static char i2c_transfer_byte(unsigned char buf)
{
	char i;
	char ack;

	for(i = 0; i < 8; i++) {
		sda = !!((buf << i) & BIT(7));
		_nop_();
		scl = 1;
		udelay(5);
		scl = 0;
	}
	NOP(3);
	sda = 1;
	NOP(3);
	scl = 1;
	NOP(4);
	ack = !sda;
	scl = 0;
	NOP(2);

	return ack;
}

static char i2c_read_byte(void)
{
	unsigned char buf = 0;
	char i;

	sda = 1;
	for(i = 0; i < 8; i++) {
		_nop_();
		scl = 0;
		udelay(5);
		scl = 1;
		NOP(3);
		buf <<= 1;
		if (sda)
			buf |= BIT(0);
		NOP(3);
	}
	scl = 0;
	NOP(3);

	return buf;
}

static void i2c_master_ack(char ack)
{
	sda = ack;
	NOP(4);
	scl = 1;
	udelay(5);
	scl = 0;
	NOP(2);
}

char i2c_transfer(char slave_addr, char addr, void *buf, unsigned char len)
{
	char i;
	char *tx = (char *)buf;

	i2c_start();
	if (!i2c_transfer_byte(slave_addr << 1))
		return -1;

	if (!i2c_transfer_byte(addr))
		return -1;

	for(i = 0; i < len; i++)
		if (!i2c_transfer_byte(*tx++))
			return -1;

	i2c_stop();

	return 0;
}

char i2c_read(char slave_addr, char addr, void *buf, unsigned char len)
{
	char i;
	char *rx = (char *)buf;

	slave_addr <<= 1;
	i2c_start();
	if (!i2c_transfer_byte(slave_addr))
		return -1;

	if (!i2c_transfer_byte(addr))
		return -1;

	i2c_start();
	if (!i2c_transfer_byte(slave_addr | BIT(0)))
		return -1;

	for(i = 0; i < len; i++) {
		*rx++ = i2c_read_byte();
		i2c_master_ack(i == len - 1);
	}

	i2c_stop();

	return 0;
}

void i2c_init(void)
{
	P1M1 &= ~GENMASK(5, 4);
	P1M0 |= GENMASK(5, 4);
}
