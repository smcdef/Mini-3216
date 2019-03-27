/**
 * Author:   smcdef <smcdef@163.com>
 * Date:     2017-05-10
 * Describe: STC EEPROM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "core.h"

#define STC15F2K60S2_EEPROM_ADDR_BASE     0x0200
#define STC15W4K56S4_EEPROM_ADDR_BASE     0x0600
#define IAP15W4K58S4_EEPROM_ADDR_BASE     0xE600
#define IAP15W4K61S4_EEPROM_ADDR_BASE     0xF200

#define EEPROM_ADDR_BASE                  STC15F2K60S2_EEPROM_ADDR_BASE
#define EEPROM_SECTOR1_ADDR               EEPROM_ADDR_BASE
#define EEPROM_SECTOR2_ADDR               (EEPROM_SECTOR1_ADDR - 0x0200)
#define EEPROM_SECTOR3_ADDR               (EEPROM_SECTOR2_ADDR - 0x0200)
#define EEPROM_SECTOR4_ADDR               (EEPROM_SECTOR3_ADDR - 0x0200)
#define EEPROM_SECTOR5_ADDR               (EEPROM_SECTOR4_ADDR - 0x0200)
#define EEPROM_SECTOR6_ADDR               (EEPROM_SECTOR5_ADDR - 0x0200)
#define EEPROM_SECTOR7_ADDR               (EEPROM_SECTOR6_ADDR - 0x0200)
#define EEPROM_SECTOR8_ADDR               (EEPROM_SECTOR7_ADDR - 0x0200)
#define EEPROM_SECTOR9_ADDR               (EEPROM_SECTOR8_ADDR - 0x0200)
#define EEPROM_SECTOR10_ADDR              (EEPROM_SECTOR9_ADDR - 0x0200)

#ifndef CONFIG_MAIN_FOSC
#error "CONFIG_MAIN_FOSC should be defined! But you not."
#endif

void eeprom_erase(unsigned int addr);
int eeprom_write(unsigned int addr, const void *src, int size);
int eeprom_read(unsigned int addr, void *des, int size);

#endif
