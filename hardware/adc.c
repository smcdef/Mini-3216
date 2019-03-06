#include "adc.h"
#include "delay.h"

void adc_init(unsigned char channel)
{
	channel &= 0x07;
	/* P1ASF |= BIT(channel); */
	ADC_CONTR = ADC_SPEEDHH;
	_nop_();
	ADC_CONTR |= channel;
	_nop_();
	ADC_CONTR |= ADC_POWER;
	EADC = 1;

	mdelay(1);
}

void adc_start(unsigned char channel)
{
	ADC_CONTR |= channel | ADC_START;
}

unsigned char adc_read(unsigned char channel)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDHH | channel | ADC_START;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR & ADC_FLAG));
	ADC_CONTR &= ~ADC_FLAG;

	return ADC_RES;
}
