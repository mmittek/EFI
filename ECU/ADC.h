#ifndef _HAVE_ADC_H_
#define _HAVE_ADC_H_

#include <avr/io.h>
#include <stdint.h>
#include "hack.h"


void ADC_Init();
uint16_t ADC_Convert(uint8_t chn);


#endif // _HAVE_ADC_H_
