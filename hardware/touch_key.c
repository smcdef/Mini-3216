#include "touch_key.h"
#include "delay.h"

sbit Data  = P5 ^ 5;
sbit Clock = P5 ^ 4;

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

	*key_value = ~dat & (KEY_ENTER | KEY_LEFT | KEY_RIGHT);

	return (dat >> 4) & (BIT(0) | BIT(1) | BIT(2));
}