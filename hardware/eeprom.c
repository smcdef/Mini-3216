/**
 * Author:   smcde <smcdef@163.com>
 * Date:     2017-05-10
 * Describe: STC EEPROM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "eeprom.h"

#if MAIN_FOSC >= 24000000L
#define ENABLE_IAP 0x80
#elif MAIN_FOSC >= 20000000L
#define ENABLE_IAP 0x81
#elif MAIN_FOSC >= 12000000L
#define ENABLE_IAP 0x82
#elif MAIN_FOSC >= 6000000L
#define ENABLE_IAP 0x83
#elif MAIN_FOSC >= 3000000L
#define ENABLE_IAP 0x84
#elif MAIN_FOSC >= 2000000L
#define ENABLE_IAP 0x85
#elif MAIN_FOSC >= 1000000L
#define ENABLE_IAP 0x86
#else
#define ENABLE_IAP 0x87
#endif

static void eeprom_close(void)
{
	IAP_CONTR = 0;
	IAP_CMD   = 0;
	IAP_TRIG  = 0;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0;
}

static unsigned char __eeprom_read(unsigned int addr)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD   = 0x01;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_TRIG  = 0x5A;
	IAP_TRIG  = 0xA5;
	_nop_();
	eeprom_close();

	return IAP_DATA;
}

static void __eeprom_write(unsigned int addr, unsigned char dat)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD   = 0x02;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_DATA  = dat;
	IAP_TRIG  = 0x5A;
	IAP_TRIG  = 0xA5;
	_nop_();
	eeprom_close();
}

/**
 * [eeprom_erase : Sector erase]
 *
 * @param addr [Address of sector erase from]
 */
void eeprom_erase(unsigned int addr)
{
	IAP_CONTR  = ENABLE_IAP;
	IAP_CMD    = 0x03;
	IAP_ADDRL  = addr;
	IAP_ADDRH  = addr >> 8;
	IAP_TRIG   = 0x5A;
	IAP_TRIG   = 0xA5;
	_nop_();
	eeprom_close();
}

/**
 * [eeprom_write : write n bytes to eeprom]
 *
 * @param  addr      [Eeprom address to write from]
 * @param  src       [Data address to write from]
 * @param  size      [Data size(byte), it must how many bytes to write]
 *
 * @return           [Data size(byte)]
 */
int eeprom_write(unsigned int addr, const void *src, int size)
{
	char *psr = (char *)src;
	int i;

	eeprom_erase(addr);
	for (i = 0; i < size; ++i)
		__eeprom_write(addr++, *psr++);

	return size;
}

/**
 * [eeprom_read : read n bytes from eeprom]
 *
 * @param  addr      [Eeprom address to read from]
 * @param  src       [Buffer address to read from]
 * @param  size      [Buffer size(byte) to read, it must how many bytes to read]
 *
 * @return           [Buffer size(byte) to read]
 */
int eeprom_read(unsigned int addr, void *des, int size)
{
	char *psr = (char *)des;
	int i;

	for (i = 0; i < size; ++i)
		*psr++ = __eeprom_read(addr++);

	return size;
}



