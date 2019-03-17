#include "touch_key.h"
#include "delay.h"

#ifdef CONFIG_BS813A
sbit key1 = P5 ^ 4;
sbit key2 = P5 ^ 5;
sbit key3 = P1 ^ 2;

char touch_key_read(char *key_value)
{
	char count = 0, value = 0;

	if (key1 == 0) {
		count++;
		value |= KEY_ENTER;
	}
	if (key2 == 0) {
		count++;
		value |= KEY_RIGHT;
	}
	if (key3 == 0) {
		count++;
		value |= KEY_LEFT;
	}

	if (key_value)
		*key_value = value;

	return count;
}

void touch_key_init(void)
{
}
#else
sbit unused = P1 ^ 2;
sbit Data   = P5 ^ 5;
sbit Clock  = P5 ^ 4;

static char key_read_byte(void)
{
	char i;
	unsigned char dat = 0;

	for(i = 0; i < 8; i++) {
		Clock = 0;
		udelay(10);
		dat >>= 1;
		Clock = 1;
		if(Data)
			dat |= 0x80;
		udelay(10);
	}

	return dat;
}

char touch_key_read(char *key_value)
{
	char dat = key_read_byte();
	char value;

	value = ~dat & (KEY_ENTER | KEY_LEFT | KEY_RIGHT);
	if (key_value)
		*key_value = value;

	return (dat >> 4) & GENMASK(2, 0);
}

void touch_key_init(void)
{
	unused = 0;
}
#endif /* CONFIG_BS813A */
