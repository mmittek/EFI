#include <avr/io.h>

#include "hack.h"

#include <avr/interrupt.h>
#include <string.h>

#include "ADC.h"
#include "USART.h"

uint16_t meas;

ISR(TIMER0_OVF_vect) {
//	reti();
}

ISR(TIMER1_OVF_vect) {
//	TIFR1 &=~(1<<TOV1);
//	reti();
}

ISR(TIMER1_COMPA_vect) {
	//PORTB ^=(1<<5);
	TCNT1H = 0;
	TCNT1L = 0;
}

volatile uint8_t crankEvents[5];
volatile uint8_t crankEventsi = 0;

// Port D interrupt handler
ISR(PCINT2_vect) {
	// Checking if I am on the rising edge
	if(PIND & (1<<2)) {
		crankEvents[crankEventsi] = TCNT0;
		TCNT0=0;
		crankEventsi=(crankEventsi+1)%5;
	}
}

int main() {
	char out[32];


	USART_Init();
	ADC_Init();

	cli();
	DDRB |= (1<<5);
	PORTB |= (1<<5);

/*
	TCCR0A = 0;
	TIMSK0 = 1<<TOIE1;
	TCCR0B = (1<<CS02) ;//| (1<<CS00);
*/


	// Timer 0 to measure the number of crank events
	DDRD &= ~(1<<4);	// input on PD4
	TCCR0A = 0;	// normal mode
	TCCR0B = (1<<CS02) | (1<<CS01) ;//| (1<<CS00);	// rising edge on T0/PD4
	TCNT0 = 0;

	// Pin on cam events
	DDRD &= ~(1<<2);	// input on PD2
	PCMSK2 |= (1<<PCINT18);	// interrupt on PD2
	PCICR |= (1<<PCIE2);	// interrupt enable on port D


	TCCR1A = 0;	// normal mode
	OCR1AH = 0;
	OCR1AL = 195;
	TIMSK1 = (1<<TOIE1) | (1<<OCIE1A);	// on overflow and compare
	TCCR1B = (1<<WGM12) | (1<<CS11);



	sei();


	while(1) {
		PORTB |= (1<<5);
		meas = ADC_Convert(1);
		PORTB &= ~(1<<5);

		sprintf(out, "Meas: %d\n", meas);
		USART_Print(out);

		for(int i=0; i<5; i++) {
			sprintf(out, "T: %d\n", crankEvents[i]);
			USART_Print(out);
		}
	}
}
