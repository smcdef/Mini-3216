/**
 * Author:   smcdef <smcdef@163.com>
 * Date:     2017-05-08
 * Describe: STC15 timer driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "timer.h"

/* 10ms@27.000MHz */
void timer0_init(void)
{
	AUXR &= 0x7F;
	TMOD &= 0xF0;
	TL0   = 0x1C;
	TH0   = 0xA8;
	TF0   = 0;
	TR0   = 1;
	ET0   = 1;
}

/* 100us@27.000MHz */
void timer1_init(void)
{
	AUXR &= 0xBF;
	TMOD &= 0x0F;
	TL1 = 0x1F;
	TH1 = 0xFF;
	TF1 = 0;
	ET1 = 1;
	TR1 = 0;
}
