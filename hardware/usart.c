#include "usart.h"

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

bit busy;

#define FOSC 26987000L

#define DISP_STATE_TIME	1
#define	DISP_STATE_TEMP	2
#define DISP_STATE_CHANGING	9

void Uart_init(void)
{
	SCON = 0x50;		//8bit and variable baudrate
	AUXR |= 0x40;		//Timer1's clock is Fosc (1T)
	AUXR &= 0xFE;		//Use Timer1 as baudrate generator
	TMOD &= 0x0F;		//Set Timer1 as 16-bit auto reload mode
	TL1 = 0x8F;		//Initial timer value
	TH1 = 0xFD;		//Initial timer value
	ET1 = 0;		//Disable Timer1 interrupt
	TR1 = 1;		//Timer1 running
    ES = 0;                     //使能串口中断
}

//?????????
void SendData(uchar dat)
{
    SBUF = dat;             //???????
	while(!TI);
	TI=0;                   //??????
}

//????????
void SendString(char *s)
{
    while (*s)              //?????????
    {
        SendData(*s++);     //????
    }
}



