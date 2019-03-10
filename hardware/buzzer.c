/**
 * Author:   smcdef <smcdef@163.com>
 * Date:     2017-05-08
 * Describe: buzzer driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "buzzer.h"
#include "delay.h"

sbit buzzer = P1 ^ 7;

static xdata unsigned char mode = 0;
static xdata unsigned char beep_time = 0, beep_count = 0;
static xdata bool finish = false;

void buzzer_power_on(void)
{
	char i;

	for (i = 0; i < 60; ++i) {
		buzzer = ~buzzer;
		udelay(600);
	}

	for (i = 0; i < 100; ++i) {
		buzzer = ~buzzer;
		udelay(480);
	}

	for (i = 0; i < 100; ++i) {
		buzzer = ~buzzer;
		udelay(180);
	}
	buzzer = 1;
}

void buzzer_enter(void)
{
	TL1 = 0xBA;
	TH1 = 0xFA;
	beep_time = 50;
	TR1 = 1;
	finish = 1;
	mode = 1;
}

void buzzer_key(void)
{
	TL1 = 0x5D;
	TH1 = 0xFD;
	beep_time = 100;
	finish = 0;
	TR1 = 1;
	mode = 1;
}

void buzzer_alarm(void)
{
	TL1 = 0x7C;
	TH1 = 0xFC;
	beep_time = 100;
	TR1 = 1;
	mode = 0;
}

/**
 * Don't try to understand the code below, because it's
 * really junk and confusing.
 */
void timer1_isr() interrupt 3 using 3
{
	static xdata unsigned char count = 0;
	static xdata bool flag = false;

	if (mode == 1) {
		buzzer = ~buzzer;
		count++;
		if (count > beep_time) {
			count = 0;
			if (finish)
				beep_count++;
			TR1 = 0;
			buzzer = 1;
			flag = true;
		}

		if (finish && flag) {
			flag = false;
			switch (beep_count) {
			case 1:
				TL1 = 0x5D;
				TH1 = 0xFD;
				beep_time = 100;
				TR1 = 1;
				break;
			case 2:
				TL1 = 0xBA;
				TH1 = 0xFA;
				beep_time = 50;
				TR1 = 1;
				break;
			case 3:
				beep_count = 0;
				finish = 0;
				break;
			default:
				break;
			}
		}
	} else if (mode == 0) {
		if (beep_count != 1 && beep_count != 3 && beep_count != 5)
			buzzer = ~buzzer;
		count++;
		if (count >= beep_time) {
			count = 0;
			beep_count++;
			TR1 = 0;
			buzzer = 1;
			flag = true;
		}
		if (flag) {
			flag = false;
			switch (beep_count) {
			case 1:
				TL1 = 0x46;
				TH1 = 0x24;
				beep_time = 25;
				buzzer = 1;
				TR1 = 1;
				break;
			case 2:
				TL1 = 0x7C;
				TH1 = 0xFC;
				beep_time = 100;
				TR1 = 1;
				break;
			case 3:
				TL1 = 0x46;
				TH1 = 0x24;
				beep_time = 25;
				buzzer = 1;
				TR1 = 1;
				break;
			case 4:
				TL1 = 0x7C;
				TH1 = 0xFC;
				beep_time = 100;
				TR1 = 1;
				break;
			case 5:
				TL1 = 0x46;
				TH1 = 0x24;
				beep_time = 25;
				buzzer = 1;
				TR1 = 1;
				break;
			case 6:
				TL1 = 0x3E;
				TH1 = 0xFE;
				beep_time = 200;
				TR1 = 1;
				break;
			case 7:
				beep_count = 0;
				finish = 0;
				break;
			default:
				break;
			}
		}
	}
}
