#ifndef _ADC_H__
#define _ADC_H__

#include "core.h"

#define ADC_POWER               0x80
#define ADC_FLAG                0x10
#define ADC_START               0x08
#define ADC_SPEEDLL             0x00
#define ADC_SPEEDL              0x20
#define ADC_SPEEDH              0x40
#define ADC_SPEEDHH             0x60

void adc_init(unsigned char channel);
void adc_start(unsigned char channel);
unsigned char adc_read(unsigned char channel);

#endif
