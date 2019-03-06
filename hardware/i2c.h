#ifndef __I2C_H__
#define __I2C_H__

#include "core.h"

char i2c_transfer(char slave_addr, char addr, char *buf, unsigned char len);
char i2c_read(char slave_addr, char addr, char *buf, unsigned char len);
void i2c_init(void);

#endif