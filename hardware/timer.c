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

void timer0_init(void)
{
	AUXR &= 0x7F;
	TMOD &= 0xF0;
	TL0   = 0xF0;
	TH0   = 0xD8;
	TF0   = 0;
	TR0   = 1;
	ET0   = 1;
}
