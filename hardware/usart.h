#ifndef __USART_H__
#define __USART_H__

#include "core.h"

void uart_init(void);
void uart_putc(char dat);
void uart_puts(char *str);

#endif
