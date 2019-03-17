#ifndef _ADC_H__
#define _ADC_H__

#include "core.h"

void adc_init(unsigned char channel);
void adc_start(unsigned char channel);
unsigned char adc_read(unsigned char channel);

#endif
