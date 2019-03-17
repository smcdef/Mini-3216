#include "adc.h"
#include "delay.h"

#define ADC_POWER               0x80
#define ADC_START               0x08
#define ADC_SPEEDLL             0x00
#define ADC_SPEEDL              0x20
#define ADC_SPEEDH              0x40
#define ADC_SPEEDHH             0x60

void adc_init(unsigned char channel)
{
	channel &= GENMASK(2, 0);
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
	ADC_CONTR &= ~GENMASK(2, 0);
	ADC_CONTR |= channel | ADC_START;
}

unsigned char adc_read(unsigned char channel)
{
	channel &= GENMASK(2, 0);
	ADC_CONTR = ADC_POWER | ADC_SPEEDHH | channel | ADC_START;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR & ADC_FLAG));
	ADC_CONTR &= ~ADC_FLAG;

	return ADC_RES;
}
