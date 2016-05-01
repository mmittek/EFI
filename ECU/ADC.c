#include "ADC.h"
#include <avr/interrupt.h>

volatile uint8_t ADC_ConversionDone = 0;
volatile uint16_t ADC_Measurement = 0;


ISR(ADC_vect) {
	cli();
	ADC_Measurement =   ADCL;
	ADC_Measurement |= (ADCH<<8);
	ADC_ConversionDone = 1;
	sei();
}

void ADC_Init() {
	ADMUX = (1<<REFS0);	 // set the reference to vcc
	ADCSRA = (1<<ADEN);	// enable the ADC
	ADCSRA |=   (1<<ADIE) |(1<<ADPS1) | (1<<ADPS2) ;//| (1<<ADPS0);	//  clk/128
	ADCSRB = 0;	// free running
	DIDR0 = 0;	// all enabled?
}

uint16_t ADC_Convert(uint8_t chn) {
	ADMUX &= ~( (1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3));	// clear all
	ADMUX |= chn&0x07;	// mask the pins
	ADC_ConversionDone = 0;
	ADCSRA |= (1<<ADSC);	// start and enable interrupts
	while( !ADC_ConversionDone) {
		// wait here
	}
	return ADC_Measurement;
}
