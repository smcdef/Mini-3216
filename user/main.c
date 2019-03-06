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
#include <string.h>

#define ADC_CHANNEL			6
#define DEFAULT_BRIGHTNESS		110
#define NIGHT_MODE_BRIGHTNESS		1
#define NIGHT_MODE_FAIR_FACTOR		130

sbit is_rotate		= P1 ^ 0;
sbit photoresistance	= P1 ^ 6;
sbit buzzer		= P1 ^ 7;

struct menu {
	const char *name;
	struct menu xdata *child;
	struct menu xdata *sibling_next, *sibling_prev;
	void *private;
	void (code *operate)(void *private);
};

struct user_data {
	union timekeeping timekeeping;
	struct fb_info fb_info;
	bool night_mode;
	struct {
		unsigned char brightness;
	} settings;
	unsigned char offset;
	struct menu xdata *current;
	bool force_update;
	char key;
};

static struct menu xdata root_menu;
static struct menu xdata mode_menu;
static struct menu xdata brightness_menu;
static struct menu xdata exit_menu;
static struct menu xdata setup_time_menu;

static struct user_data pdata user_data;

static void user_data_init(struct user_data *user)
{
	memset(user, 0, sizeof(*user));
	user->fb_info.brightness = DEFAULT_BRIGHTNESS;
	user->settings.brightness = DEFAULT_BRIGHTNESS;
	user->fb_info.fair = false;
}

static void pca_init(void)
{
	CCON	= 0;
	CL	= 0;
	CH	= 0;
	CMOD	= 0x00;
	CCAPM0	= 0x21;
	CCAPM1	= 0x31;
	CR	= 1;
}

static void local_irq_disable(void)
{
	EA = 0;
}

static void local_irq_enable(void)
{
	EA = 1;
}

static unsigned char fb_load_temperature(unsigned char offset)
{
	char str[] = {
		' ', '-', ' ', '-', ' ', '.',
		' ', '-', ' ', 'c', '\0',
	};
	char integer, decimals;
	char *p = str + 1;

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
	return fb_set_string(offset, p);
}

static bool should_show_temperature(struct user_data pdata *user)
{
	return !user->night_mode;
}

static void fb_load_time(void *priv)
{
	static pdata char sec_old = 0xff, min_old = 0xff, hour_old = 0xff;
	pdata struct user_data *user = priv;
	pdata union timekeeping *timekeeping = &user->timekeeping;
	pdata struct fb_info *fb_info = &user->fb_info;
	bool force = user->force_update;

	if (ds3231_read_times(timekeeping))
		return;

	if (sec_old != timekeeping->time.sec ||
	    min_old != timekeeping->time.min ||
	    hour_old != timekeeping->time.hour ||
	    force) {
		char str[5];
		char half_low;
		static bool is_temp = false;
		unsigned char offset = 0;

		if (force)
			user->force_update = false;

		sec_old = timekeeping->time.sec;
		if (hour_old != timekeeping->time.hour || force) {
			hour_old = timekeeping->time.hour;

			str[0] = hour_old / 16 + '0';
			str[1] = ' ';
			str[2] = hour_old % 16 + '0';
			str[3] = ' ';
			str[4] = '\0';
			offset += fb_set_string(offset, str);
		} else {
			offset += 14;
		}

		if (sec_old & BIT(0))
			str[0] = '^';
		else
			str[0] = 'v';
		str[1] = ' ';
		str[2] = '\0';
		offset += fb_set_string(offset, str);

		if (min_old != timekeeping->time.min || force) {
			min_old = timekeeping->time.min;

			str[0] = min_old / 16 + '0';
			str[1] = ' ';
			str[2] = min_old % 16 + '0';
			str[3] = '\0';
			offset += fb_set_string(offset, str);
		} else {
			offset += 13;
		}

		half_low = sec_old & 0x0f;
		if (half_low > 2 && half_low < 5 &&
		    should_show_temperature(user)) {
			if (!is_temp) {
				unsigned char brightness = fb_info->brightness;

				fb_info->brightness >>= 1;
				fb_info->brightness += 1;
				fb_load_temperature(offset);
				fb_scan(fb_info, 64, 1);
				fb_info->brightness = brightness;
				is_temp = true;
			}
			fb_info->offset = MATRIXS_COLUMNS;
			user->offset = 0;
		} else {
			if (is_temp) {
				unsigned char brightness = fb_info->brightness;

				fb_info->brightness >>= 1;
				fb_info->brightness += 1;
				fb_scan_reverse(fb_info, 64, 1);
				fb_info->brightness = brightness;
				is_temp = false;
			}
			fb_info->offset = 0;
			user->offset = MATRIXS_COLUMNS;
		}
	}
}

static void set_times(void *priv)
{
	char key = *(char *)priv;
	static enum set_type type = SET_YEAR;
	static const char *name = { "年", "月", "日", "时", "分", };

	switch (key) {
	case KEY_RIGHT:
		break;
	case KEY_LEFT:
		break;
	case KEY_ENTER:
		if (--type == SET_DAY)
			type--;
		if ((char)type == -1)
			type = SET_YEAR;
		break;
	default:
		break;
	}
}

#define ROOT_MENU_NAME		"root"

static bool is_root_menu(struct menu xdata *entry)
{
	return !strcmp(entry->name, ROOT_MENU_NAME);
}

static void menu_init(void)
{
	memset(&root_menu, 0, sizeof(root_menu));
	root_menu.name = ROOT_MENU_NAME;
	root_menu.child = &mode_menu;
	root_menu.private = &user_data;
	root_menu.operate = fb_load_time;

	memset(&mode_menu, 0, sizeof(mode_menu));
	mode_menu.name = "模式";
	mode_menu.private = &user_data;
	mode_menu.sibling_next = &brightness_menu;
	mode_menu.sibling_prev = &exit_menu;

	memset(&brightness_menu, 0, sizeof(brightness_menu));
	brightness_menu.name = "时间";
	brightness_menu.private = &user_data;
	brightness_menu.sibling_next = &exit_menu;
	brightness_menu.sibling_prev = &mode_menu;

	memset(&exit_menu, 0, sizeof(exit_menu));
	exit_menu.name = "退出";
	exit_menu.child = &root_menu;
	exit_menu.private = &user_data;
	exit_menu.sibling_next = &mode_menu;
	exit_menu.sibling_prev = &brightness_menu;

	memset(&setup_time_menu, 0, sizeof(setup_time_menu));
	setup_time_menu.name = "设置时间 2 0 ";
	setup_time_menu.private = &user_data.key;

	user_data.current = &root_menu;
}

static bool interface_switching(struct user_data pdata *user, char key)
{
	struct fb_info pdata *fb_info = &user->fb_info;
	struct menu xdata *current = user->current;
	bool success = false;

	switch (key) {
	case KEY_ENTER:
		if (is_root_menu(current) || !current->child)
			break;

		current = current->child;
		if (is_root_menu(current)) {
			user->offset = 0;
			user->force_update = true;
			break;
		}
		user->offset += fb_set_string(user->offset, current->name);
		fb_scan(fb_info, 64, 1);
		fb_info->offset += MATRIXS_COLUMNS;
		break;
	case KEY_LEFT:
		if (current->sibling_prev) {
			user->offset += fb_set_string(user->offset,
						current->sibling_prev->name);
			fb_scan_reverse(fb_info, 64, 1);
			fb_info->offset -= MATRIXS_COLUMNS;
			current = current->sibling_prev;
		}
		break;
	case KEY_RIGHT:
		if (current->sibling_next) {
			user->offset += fb_set_string(user->offset,
						current->sibling_next->name);
			fb_scan(fb_info, 64, 1);
			fb_info->offset += MATRIXS_COLUMNS;
			current = current->sibling_next;
		}
		break;
	case KEY_LEFT | KEY_RIGHT:
		/* special for root menu and enter the settings menu */
		if (!is_root_menu(current))
			break;

		current = current->child;

		user->offset += fb_set_string(user->offset, current->name);
		fb_scan(fb_info, 64, 1);
		fb_info->offset += MATRIXS_COLUMNS;
		break;
	case KEY_LEFT | KEY_ENTER:
		/* special for root menu and enter the setup time menu */
		if (!is_root_menu(current))
			break;
		current = &setup_time_menu;
		fb_info->offset = fb_scan_string(fb_info, 2, user->offset,
						 current->name);
		break;
	default:
		return false;
	}
	if (user->current != current) {
		success = true;
		user->current = current;
	}

	return success;
}

void main(void)
{
	struct menu xdata *current = user_data.current;
	struct fb_info pdata *fb_info = &user_data.fb_info;

	Uart_init();
	user_data_init(&user_data);
	//fb_off();
	fb_clear(0, 64);
	i2c_init();
	ds3231_init();
	font_sort();
	menu_init();
	pca_init();
	adc_init(ADC_CHANNEL);
	local_irq_enable();

	while (1) {
		char key;

		adc_start(ADC_CHANNEL);
		if (touch_key_read(&key)) {
			bool success = interface_switching(&user_data, key);

			while (success && touch_key_read(&key))
				fb_show(fb_info);
			current = user_data.current;
			if (success)
				user_data.key = 0;
		}
		if (current && current->operate)
			current->operate(current->private);
		fb_show(fb_info);
	}
}

/* Timer0 interrupt routine */
void timer0_isr() interrupt 1 using 2
{

}

void adc_isr(void) interrupt 5 using 1
{
	unsigned char result;

	ADC_CONTR &= ~ADC_FLAG;
	result = ADC_RES;
	if (result > 0xf0) {
		user_data.night_mode = true;
		user_data.fb_info.fair = NIGHT_MODE_FAIR_FACTOR;
		user_data.fb_info.brightness = NIGHT_MODE_BRIGHTNESS;
	} else if (result < 0xc8) {
		user_data.night_mode = false;
		user_data.fb_info.brightness = user_data.settings.brightness;
		user_data.fb_info.fair = false;
	}
}

void pca_isr(void) interrupt 7 using 2
{
	if (CCF0) {
		CCF0 = 0;
	} if (CCF1) {
		CCF1 = 0;
		user_data.fb_info.rotate = !is_rotate;
	}
}
