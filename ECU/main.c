#include <avr/io.h>

#include "hack.h"

#include <avr/interrupt.h>
#include <string.h>

#include "ADC.h"
#include "USART.h"

uint32_t ticks = 0;

uint16_t meas;

volatile uint16_t ticksPerDegree = 0;
volatile uint16_t nextEventAt = 0;
volatile uint16_t now = 0;
volatile uint16_t rpm = 0;
volatile uint8_t running = 0;


ISR(TIMER1_OVF_vect) {
	// Restart the cycle  high before low on write
	TCNT1H = 0;
	TCNT1L = 0;
/*
	OCR1AH = 0;
	OCR1AL = 200;
	TCNT1H = 0;
	TCNT1L = 0;
	nextEventAt = 200;
	*/
}

volatile uint16_t cycleTime = 0;

volatile int16_t cycleAngle =0;


uint16_t ignitionAngles[] = {90, 270, 540, 360};
uint8_t ignitionTeeth[] = {3, 9, 18, 12};
volatile uint16_t prevCrankEvent = 0;
volatile uint16_t ticksPerRotation = 0;
volatile uint8_t cycleCrankEvents = 0;
volatile uint8_t syncCrankEvents = 0;
volatile uint8_t crankEventsArr[5];
volatile uint8_t crankEvents = 0;
volatile uint8_t crankEventsArri = 0;


ISR(TIMER0_OVF_vect) {
	running = 0;
}

volatile uint32_t ref = 0;

// Execution timer interrupt with resolution of
// 10th of millisecond (100us)
ISR(TIMER0_COMPA_vect) {

	cycleTime++;
	uint32_t angle = 90;
	ref = (( (uint32_t)ticksPerRotation/25)*angle)/90;

	if(!running) return;
	if(cycleTime == ref-10) {
		PORTB |=(1<<5);
	}

	if(cycleTime == ref) {
		PORTB &=~(1<<5);
	}


}


ISR(TIMER1_COMPB_vect) {
	/*
	now = TCNT1L + ( TCNT1H<<8 );
	nextEventAt = now + ( OCR1AL + (OCR1AH<<8) );
	PORTB ^=(1<<5);
	*/
}


uint16_t getNowTicks() {
	uint16_t now = TCNT1L;
	now |=  ( TCNT1H<<8 );
	return now;
}

inline uint16_t convertToT(uint16_t in) {
	return (in/25)<<2;
}


// Crank events
ISR(PCINT0_vect) {
	uint16_t now;
	uint16_t diff;
	// Checking if I am on the rising edge of PB0
	if(PINB & (1<<0)) {
		crankEvents++;
		cycleCrankEvents++;
		syncCrankEvents++;
		now = getNowTicks();
		diff = (now - prevCrankEvent);
		ticksPerRotation = diff*12;
		rpm = 60000/((2*ticksPerRotation)/125);
		prevCrankEvent = now;
	}
}

// Port D interrupt handler
ISR(PCINT2_vect) {
	// Checking if I am on the rising edge PD2
	if(PIND & (1<<2)) {
		crankEventsArr[crankEventsArri] = syncCrankEvents;
		crankEventsArri=(crankEventsArri+1)%5;
		switch(syncCrankEvents) {
			case 10:
			break;
			case 2:
			break;
			case 12:
				running = 1;
				cycleTime = 0;
				cycleCrankEvents = 0;
			break;
		}
		syncCrankEvents = 0;
	}
}

int main() {
	char out[32];



	cli();
	DDRB |= (1<<5);
	PORTB &= ~(1<<5);

/*
	TCCR0A = 0;
	TIMSK0 = 1<<TOIE1;
	TCCR0B = (1<<CS02) ;//| (1<<CS00);
*/

	// Setting coil pins
	DDRB |= (1<<1);	// PB1 COIL1
	PORTB &= ~(1<<1);	// set 0



	// Timer 0 to measure the number of crank events
/*
	DDRD &= ~(1<<4);	// input on PD4
	TCCR0A = 0;	// normal mode
	TCCR0B = (1<<CS02) | (1<<CS01);// | (1<<CS00);	// falling edge on T0/PD4
	TCNT0 = 0;
*/


	// Pin on cam events PD2
	DDRD &= ~(1<<2);	// input on PD2
	PCMSK2 |= (1<<PCINT18);	// interrupt on PD2
	PCICR |= (1<<PCIE2);	// interrupt enable on port D

	// Pin on crank events: PB0
	DDRB &= ~(1<<0);	// input on PB0
	PCMSK0 |= (1<<PCINT0);	// interrupt on PB0
	PCICR |= (1<<PCIE0); // interrupt on port B

	// 16 bit timer 1 on 100us - main time measurement timer
	TCCR1A = 0;	// normal mode
	TIMSK1 = (1<<TOIE1) ;	// on overflow and compare
	TCCR1B =  (1<<CS12);	// /256

	// Timer 0 for 10th of ms resolution execution
	TCCR0A = (1<<WGM01);	// normal mode, CTC
	TCCR0B = (1<<CS02);	// /256
	TCNT0 = 0;
	OCR0A = 5;
	TIMSK0 = (1<<OCIE0A) | (1<<TOIE0);	// compare and overflow

	sei();

	// Higher level components
	USART_Init();
	ADC_Init();


	while(1) {

		meas = ADC_Convert(1);

//		if(running) {

			sprintf(out, "Meas: %d\n", meas);
			USART_Print(out);

			sprintf(out, "Cycle ms/10s: %d\n", 2*convertToT(ticksPerRotation));
			USART_Print(out);

			sprintf(out, "Rotation ticks: %d\n", ticksPerRotation);
			USART_Print(out);

			sprintf(out, "RPM: %d\n", rpm);
			USART_Print(out);

			sprintf(out, "Ticks per degree: %d\n", ticksPerDegree);
			USART_Print(out);

			sprintf(out, "ref: %d\n", ref);
			USART_Print(out);


			for(int i=0; i<5; i++) {
				sprintf(out, "T: %d\n", crankEventsArr[i]);
				USART_Print(out);
			}
//		}
	}
}
