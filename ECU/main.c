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


int main() {
	char out[32];


	USART_Init();
	ADC_Init();

	cli();
	DDRB |= (1<<5);
	PORTB |= (1<<5);


	TCCR0A = 0;
	TIMSK0 = 1<<TOIE1;
	TCCR0B = (1<<CS02) ;//| (1<<CS00);


	TCCR1A = 0;	// normal mode

	OCR1AH = 0;
	OCR1AL = 195;

	TIMSK1 = (1<<TOIE1) | (1<<OCIE1A);	// on overflow and compare
	TCCR1B = (1<<WGM12) | (1<<CS11);



	sei();


	while(1) {
		meas = ADC_Convert(1);

		sprintf(out, "Meas: %d\n", meas);
		USART_Print(out);
	}
}
