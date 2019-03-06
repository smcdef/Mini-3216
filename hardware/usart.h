#ifndef __USART_H__
#define __USART_H__

#include "core.h"

//typedef unsigned char BYTE;
//typedef unsigned int WORD;



//#define FOSC 26987000L          //系统频率
#define BAUD 115200             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位


void Uart_init(void);
void SendData(unsigned char dat);
void SendString(char *s);



#endif

