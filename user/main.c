/**
 * Core interface implementation for Mini3216.
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

#include "fb_core.h"
#include "fb_mem.h"
#include "config.h"
#include "font.h"
#include "adc.h"
#include "eeprom.h"
#include "ds3231.h"
#include "i2c.h"
#include "usart.h"
#include "lm75.h"
#include "touch_key.h"
#include "delay.h"
#include "timer.h"
#include "buzzer.h"
#include <string.h>

#define ADC_CHANNEL			6
#define DEFAULT_BRIGHTNESS		120
#define NIGHT_MODE_BRIGHTNESS		2
#define NIGHT_MODE_FAIR_FACTOR		50
#define SCAN_SPEED_BRIGHTNESS		55
#define SEETING_TIME_SCAN_SPEED		10

sbit is_rotate = P1 ^ 0;

struct set_time_env {
	char key;
	char max;
	char min;
	enum set_type type;
	const char *name;
	struct fb_info idata *fb_info;
};

struct menu {
	const char *name;
	struct menu xdata *child;
	struct menu xdata *sibling_next, *sibling_prev;
	void *private;
	void (code *operate)(void *private);
	unsigned int (code *fb_load)(unsigned int offset);
};

struct user_data {
	struct rtc rtc;
	struct fb_info fb_info;
	bool night_mode;
#ifdef CONFIG_DS3231_INT
	bool rtc_update;
#endif
	struct {
		unsigned char brightness;
		bool oscillator_on;
	} settings;
	bool fb_update;
	char key;
};

static struct menu xdata root_menu;
static struct menu xdata set_hour_menu;
static struct menu xdata set_minute_menu;
static struct menu xdata *current;

static struct user_data idata user_data;

static void local_irq_disable(void)
{
	EA = 0;
}

static void local_irq_enable(void)
{
	EA = 1;
}

#ifdef CONFIG_DS3231_INT
static bool rtc_update_set_return(bool update)
{
	bool updated;

	local_irq_disable();
	updated = user_data.rtc_update;
	user_data.rtc_update = update;
	local_irq_enable();

	return updated;
}

static void rtc_update_set_irq(bool update)
{
	user_data.rtc_update = update;
}
#else
static bool rtc_update_set_return(bool update)
{
	/* Prevent compiler warning for not using 'update' */
	update = update;

	return true;
}

static void rtc_update_set_irq(bool update)
{
	/* Prevent compiler warning for not using 'update' */
	update = update;
}
#endif

static void user_data_init(void)
{
	memset(&user_data, 0, sizeof(user_data));
	user_data.fb_info.fair = false;
	user_data.fb_info.rotate = !is_rotate;
	user_data.fb_update = true;
	rtc_update_set_return(true);
	eeprom_read(EEPROM_SECTOR1_ADDR, &user_data.settings,
		    sizeof(user_data.settings));

	if (user_data.settings.brightness > DEFAULT_BRIGHTNESS ||
	    user_data.settings.oscillator_on > 1) {
		user_data.settings.brightness = DEFAULT_BRIGHTNESS;
		user_data.settings.oscillator_on = false;
		eeprom_write(EEPROM_SECTOR1_ADDR, &user_data.settings,
			     sizeof(user_data.settings));
	}

	ds3231_enable_oscillator(user_data.settings.oscillator_on);
	user_data.fb_info.brightness = user_data.settings.brightness;
}

static void pca_init(void)
{
	CCON	= 0;
	CL	= 0;
	CH	= 0;
	CMOD	= 0x00;
#ifdef CONFIG_DS3231_INT
	CCAPM0	= 0x21;		/* Rising edge */
#endif
	CCAPM1	= 0x31;		/* Rising and falling edges */
	CR	= 1;
}

static unsigned int fb_load_temperature(unsigned int offset)
{
	char idata str[] = " - - . - c";
	char integer, decimals;
	char idata *p = str + 1;

	if (lm75_read_temperature(&integer, &decimals) &&
	    ds3231_read_temperature(&integer, &decimals))
		goto err;

	if (integer > -10) {
		if (integer > 9)
			str[1] = integer / 10 + '0';
		else if (integer >= 0)
			str[1] = NONE_ASCII;
		else
			integer = ~integer + 1;

		str[3] = integer % 10 + '0';
		str[7] = decimals / 10 + '0';
	} else {
		integer = ~integer + 1;
		str[1] = ' ';
		str[5] = integer / 10 + '0';
		str[7] = integer % 10 + '0';
		p = str;
	}
err:
	return fb_copy_string(offset, p);
}

static bool should_show_temperature(struct user_data idata *user)
{
	return !user->night_mode;
}

static bool should_chime(struct rtc *rtc)
{
	return rtc->sec == 0x58 &&
	       rtc->min == 0x59 &&
	       rtc->hour > 0x07 && rtc->hour < 0x23;
}

static void fb_load_times(void *priv)
{
	static char sec_old = 0xff, min_old = 0xff, hour_old = 0xff;
	struct user_data idata *user = priv;
	struct rtc idata *rtc = &user->rtc;
	struct fb_info idata *fb_info = &user->fb_info;
	bool force = user->fb_update;
	char idata str[5];
	char half_low;
	static bool is_temp = false;
	unsigned int offset = 0;

	if (!rtc_update_set_return(false))
		return;

	if (ds3231_read_times(rtc))
		return;

	if (sec_old == rtc->sec &&
	    min_old == rtc->min &&
	    hour_old == rtc->hour && !force)
		return;

	if (should_chime(rtc) && !user->night_mode)
		buzzer_chime();

	if (force) {
		fb_info->offset = 0;
		user->fb_update = false;
		is_temp = false;
	}

	sec_old = rtc->sec;
	if (hour_old != rtc->hour || force) {
		hour_old = rtc->hour;
		str[0] = hour_old / 16 + '0';
		str[1] = ' ';
		str[2] = hour_old % 16 + '0';
		str[3] = ' ';
		str[4] = '\0';
		offset += fb_copy_string(offset, str);
	} else {
		offset += 14;
	}

	if (sec_old & BIT(0))
		str[0] = '^';
	else
		str[0] = 'v';
	str[1] = ' ';
	str[2] = '\0';
	offset += fb_copy_string(offset, str);

	if (min_old != rtc->min || force) {
		min_old = rtc->min;
		str[0] = min_old / 16 + '0';
		str[1] = ' ';
		str[2] = min_old % 16 + '0';
		str[3] = '\0';
		offset += fb_copy_string(offset, str);
	} else {
		offset += 13;
	}

	half_low = sec_old & 0x0f;
	if (half_low > 2 && half_low < 5 && should_show_temperature(user)) {
		if (!is_temp) {
			unsigned char brightness;

			/**
			 * fb_info->brightness maybe alter in irq.
			 * So we should disable irq to protect shared
			 * variable.
			 */
			local_irq_disable();
			/**
			 * Re-check under disable irq.
			 */
			if (!should_show_temperature(user)) {
				local_irq_enable();
				return;
			}
			brightness = fb_info->brightness;
			fb_info->brightness = SCAN_SPEED_BRIGHTNESS;
			fb_load_temperature(offset);
			fb_info->offset = fb_scan(fb_info, 64, 1);
			fb_info->brightness = brightness;
			local_irq_enable();
			is_temp = true;
		}
	} else if (is_temp) {
		unsigned char brightness;

		/**
		 * fb_info->brightness maybe alter in irq.
		 * So we should disable irq to protect shared
		 * variable.
		 */
		local_irq_disable();
		brightness = fb_info->brightness;
		/**
		 * If we are in night mode, we should not increase
		 * the brightness.
		 */
		if (!user->night_mode)
			fb_info->brightness = SCAN_SPEED_BRIGHTNESS;
		fb_info->offset = fb_scan_reverse(fb_info, 64, 1);
		fb_info->brightness = brightness;
		local_irq_enable();
		is_temp = false;
	}
}

static unsigned int fb_load_time(unsigned int offset, enum set_type type,
				 const char *s)
{
	char value;
	unsigned int offset_old = offset;
	char idata str[] = "  - - ";

	if (ds3231_read_time(type, &value))
		return 0;

	str[2] = value / 16 + '0';
	str[4] = value % 16 + '0';
	offset += fb_copy_string(offset, str);
	offset += fb_copy_string(offset, s);

	return offset_old - offset;
}

static unsigned int fb_load_hour(unsigned int offset)
{
	return fb_load_time(offset, SET_HOUR, "时");
}

static unsigned int fb_load_minute(unsigned int offset)
{
	return fb_load_time(offset, SET_MINUTES, "分");
}

static void key_delay(struct fb_info *fb_info)
{
	char i;

	buzzer_key();
	for (i = 0; i < 20; i++)
		fb_show(fb_info);
}

static bool set_time_common(struct set_time_env idata *env)
{
	char value;
	enum set_type type = env->type;
	struct fb_info idata *fb_info = env->fb_info;

	if (ds3231_read_time(type, &value))
		return false;
	value = value / 16 * 10 + value % 16;

	switch (env->key) {
	case KEY_RIGHT:
		if (++value > env->max)
			value = env->min;
		key_delay(fb_info);
		break;
	case KEY_LEFT:
		if (--value < env->min)
			value = env->max;
		key_delay(fb_info);
		break;
	default:
		return false;
	}

	ds3231_set_time(type, value / 10 * 16 + value % 10);
	fb_load_time(fb_info->offset, type, env->name);

	return true;
}

static void set_hour(void *priv)
{
	struct user_data idata *user = priv;
	struct set_time_env idata env;

	env.key		= user->key;
	env.max		= 23;
	env.min		= 0;
	env.type	= SET_HOUR;
	env.name	= "时";
	env.fb_info	= &user->fb_info;

	if (!set_time_common(&env)) {
		if (!rtc_update_set_return(false))
			return;
		fb_load_hour(user->fb_info.offset);
	}
}

static void set_minute(void *priv)
{
	struct user_data idata *user = priv;
	struct set_time_env idata env;

	env.key		= user->key;
	env.max		= 59;
	env.min		= 0;
	env.type	= SET_MINUTES;
	env.name	= "分";
	env.fb_info	= &user->fb_info;

	if (!set_time_common(&env)) {
		if (!rtc_update_set_return(false))
			return;
		fb_load_minute(user->fb_info.offset);
		return;
	}
	ds3231_set_time(SET_SECOND, 0);
}

#define ROOT_MENU_NAME		"root"

static bool is_root_menu(struct menu xdata *entry)
{
	return entry->name && !strcmp(entry->name, ROOT_MENU_NAME);
}

static void menu_init(void)
{
	memset(&root_menu, 0, sizeof(root_menu));
	root_menu.name = ROOT_MENU_NAME;
	/* root_menu.child = &set_hour_menu; */
	root_menu.private = &user_data;
	root_menu.operate = fb_load_times;

	memset(&set_hour_menu, 0, sizeof(set_hour_menu));
	set_hour_menu.private = &user_data;
	set_hour_menu.child = &set_minute_menu;
	set_hour_menu.fb_load = fb_load_hour;
	set_hour_menu.operate = set_hour;

	memset(&set_minute_menu, 0, sizeof(set_minute_menu));
	set_minute_menu.private = &user_data;
	set_minute_menu.child = &root_menu;
	set_minute_menu.fb_load = fb_load_minute;
	set_minute_menu.operate = set_minute;

	current = &root_menu;
}

static bool interface_switching(struct user_data idata *user, char key)
{
	struct fb_info idata *fb_info = &user->fb_info;
	struct menu xdata *current_old = current;
	bool success = false;
#if CONFIG_FB_SIZE > 128
	unsigned int offset;
#endif

	switch (key) {
	case KEY_ENTER:
		if (is_root_menu(current) || !current->child)
			break;

		buzzer_enter();
		current = current->child;
		if (is_root_menu(current)) {
			rtc_update_set_return(true);
			user->fb_update = true;
			break;
		}

		if (current->fb_load)
			current->fb_load(fb_info->offset + MATRIXS_COLUMNS);
		else
			fb_copy_string(fb_info->offset + MATRIXS_COLUMNS,
				       current->name);
		fb_info->offset = fb_scan(fb_info, 64, 1);
		break;
	case KEY_LEFT:
		if (!current->sibling_prev)
			break;
		buzzer_key();
		fb_copy_string(fb_info->offset - MATRIXS_COLUMNS,
					current->sibling_prev->name);
		fb_info->offset = fb_scan_reverse(fb_info, 64, 1);
		current = current->sibling_prev;
		break;
	case KEY_RIGHT:
		if (!current->sibling_next)
			break;
		buzzer_key();
		fb_copy_string(fb_info->offset + MATRIXS_COLUMNS,
				current->sibling_next->name);
		fb_info->offset = fb_scan(fb_info, 64, 1);
		current = current->sibling_next;
		break;
	case KEY_LEFT | KEY_RIGHT:
		/* special for root menu and enter the settings menu */
		if (!is_root_menu(current))
			break;

		if (!current->child)
			break;
		buzzer_enter();
		current = current->child;
		fb_copy_string(fb_info->offset + MATRIXS_COLUMNS,
				current->name);
		fb_info->offset = fb_scan(fb_info, 64, 1);
		break;
	case KEY_LEFT | KEY_ENTER:
		/* special for root menu and enter the setup time menu */
		if (!is_root_menu(current))
			break;

		buzzer_enter();
		current = &set_hour_menu;
#if CONFIG_FB_SIZE > 128
		offset = fb_info->offset + MATRIXS_COLUMNS;
		offset += fb_copy_string(offset, "设置时间");
		if (current->fb_load)
			offset += current->fb_load(offset);
		fb_info->offset = fb_scan(fb_info, 128,
					  SEETING_TIME_SCAN_SPEED);
#else
		fb_info->offset = fb_scan_string(fb_info, MATRIXS_COLUMNS,
						 SEETING_TIME_SCAN_SPEED,
						 "设置时间");
		if (current->fb_load)
			current->fb_load(fb_info->offset + MATRIXS_COLUMNS);
		fb_info->offset = fb_scan(fb_info, 64, SEETING_TIME_SCAN_SPEED);
#endif
		break;
	case KEY_LEFT | KEY_RIGHT | KEY_ENTER:
		/* special for root menu and enable/disable oscillator */
		if (!is_root_menu(current))
			break;

		if (user->settings.oscillator_on)
			break;
		user->settings.oscillator_on = true;
		eeprom_write(EEPROM_SECTOR1_ADDR, &user->settings,
			     sizeof(user->settings));
		ds3231_enable_oscillator(true);
		buzzer_enter();
		return true;
	default:
		return false;
	}

	if (current_old != current)
		success = true;

	return success;
}

void main(void)
{
	struct fb_info idata *fb_info = &user_data.fb_info;

	user_data_init();
	buzzer_power_on();
	fb_off();
	fb_matrixs_test();
	uart_init();
	touch_key_init();
	fb_set(0, 0, 64);
	i2c_init();
	ds3231_init();
	font_sort();
	menu_init();
	pca_init();
	adc_init(ADC_CHANNEL);
	timer0_init();
	timer1_init();
	local_irq_enable();

	while (1) {
		if (touch_key_read(&user_data.key)) {
			bool success = interface_switching(&user_data,
							   user_data.key);

			while (success && touch_key_read(&user_data.key))
				fb_show(fb_info);
		}
		if (current && current->operate)
			current->operate(current->private);

		/**
		 * We should disable local irq when call fb_show().
		 * But we can not do this, because if we do that,
		 * the buzzer_chime() will work abnormal. And now
		 * we can do that. Because timer0 is not able to
		 * mask interrupts via EA = 0.
		 */
		local_irq_disable();
		fb_show(fb_info);
		local_irq_enable();
	}
}

/* Timer1 interrupt routine(10ms) */
void timer1_isr() interrupt 3 using 2
{
	static unsigned char timer_count = 0;

	/* every 1280ms */
	if (timer_count % 128 == 0)
		adc_start(ADC_CHANNEL);

	timer_count++;
}

#ifdef CONFIG_PRES_PULL_UP
#define NIGHT_MODE_ADC_VALUE		0xa0
#define BRIGHT_MODE_ADC_VALUE		0x60
#else
#define NIGHT_MODE_ADC_VALUE		0xf0
#define BRIGHT_MODE_ADC_VALUE		0xc8
#endif

/* ADC interrupt routine */
void adc_isr(void) interrupt 5 using 1
{
	unsigned char result;

	ADC_CONTR &= ~ADC_FLAG;
	result = ADC_RES;
	if (result > NIGHT_MODE_ADC_VALUE) {
		user_data.night_mode = true;
		user_data.fb_info.fair = NIGHT_MODE_FAIR_FACTOR;
		user_data.fb_info.brightness = NIGHT_MODE_BRIGHTNESS;
	} else if (result > BRIGHT_MODE_ADC_VALUE) {
		user_data.night_mode = true;
		user_data.fb_info.brightness = NIGHT_MODE_BRIGHTNESS;
		user_data.fb_info.fair = false;
	} else if (user_data.night_mode) {
		user_data.night_mode = false;
		user_data.fb_info.brightness = user_data.settings.brightness;
		user_data.fb_info.fair = false;
	}
}

void pca_isr(void) interrupt 7 using 2
{
	if (CCF0) {
		CCF0 = 0;
		rtc_update_set_irq(true);
	}

	if (CCF1) {
		CCF1 = 0;
		user_data.fb_info.rotate = !is_rotate;
	}
}
