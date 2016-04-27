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
volatile uint8_t crankEventsArr[5];
volatile uint8_t crankEventsArri = 0;

volatile int16_t dwellAt = 0;
volatile int16_t fireAt = 0;

volatile uint16_t syncCrankEvents = 0;
volatile uint16_t crankEvents = 0;

typedef enum {
	SYNC = 0,
	COIL0_DWELL,
	COIL0_FIRE,
	DONE,
}ECU_state;

volatile ECU_state currentState = DONE;



ISR(TIMER0_OVF_vect) {
	running = 0;
}

volatile uint32_t ref = 0;




uint16_t getNowTicks() {
	uint16_t now = TCNT1L;
	now |=  ( TCNT1H<<8 );
	return now;
}

inline uint16_t convertToT(uint16_t in) {
	return (in/25)<<2;
}


// Crank events on either! EDGE
volatile uint8_t PINB_prev =0;
volatile uint32_t crankTicks = 0;
volatile uint32_t crankTime = 0;
ISR(PCINT0_vect) {
	// Change on PB0
	if( (PINB ^ PINB_prev) & (1<<0) ) {
		crankEvents++;
		syncCrankEvents++;
		running = 1;
		crankTicks = TCNT0;
		crankTime = (crankTicks *100000)/15625;
		TCNT0 = 0;
	}
	PINB_prev = PINB;
}

ISR(TIMER2_COMPA_vect) {
	PORTB &= ~(1<<5);
	TIMSK2 = 0;	// turn off the interrupt
	TCCR2B = 0;
}

void timer2_start(uint16_t countToMs) {
	TIMSK2 |= (1<<OCIE2A);
	OCR2A = countToMs*15;
	TCNT2 = 0;
	TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);	// /1024
}

ISR(TIMER1_COMPA_vect) {
	// Transitions
	switch(currentState) {
		case SYNC:
			currentState = COIL0_DWELL;
		break;

		case COIL0_DWELL:
			currentState = COIL0_FIRE;
		break;

		default:
		case COIL0_FIRE:
			//TIMSK1 = 0;	// turn off the interrupt
			//TCCR1B = 0;	// turn off he clock source
			return;
	}

	switch(currentState) {
		case COIL0_DWELL:
			OCR1AH = fireAt>>8;
			OCR1AL = fireAt & 0xff;
			PORTB |= (1<<5);	// write one to start dwelling
		break;

		case COIL0_FIRE:
			PORTB &= ~(1<<5);	// write zero to fire
		break;
	}

}


void timer1_start(uint16_t countTo) {
	TCCR1A = 0;	// normal mode
	// high BEFORE low on write
	TCNT1H = 0;
	TCNT1L = 0;
	OCR1AH = countTo>>8;
	OCR1AL = countTo & 0xff;
	// start the timer!
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);	// ctc, /1024
	TIMSK1 = (1<<OCIE1A);
}

// Cam events
// Port D interrupt handler
volatile uint8_t PIND_prev = 0;
ISR(PCINT2_vect) {
	if( (PIND ^ PIND_prev) & (1<<2) ) {

		// Falling edge on PD2 - cam signal
		if(!( PIND & (1<<2) )) {
			if(syncCrankEvents == 24) {
				// SYNC JUST STARTED xD
				crankEvents = 0;

				currentState = SYNC;
				timer1_start( dwellAt);


			}
			syncCrankEvents = 0;
		}
	}
	PIND_prev= PIND;
}

int main() {
	char out[32];



	cli();

	// OUtput pin for testing
	DDRB |= (1<<5);
	PORTB &= ~(1<<5);

/*
	TCCR0A = 0;
	TIMSK0 = 1<<TOIE1;
	TCCR0B = (1<<CS02) ;//| (1<<CS00);
*/




	// Timer 0 to measure the number of crank events
/*
	DDRD &= ~(1<<4);	// input on PD4
	TCCR0A = 0;	// normal mode
	TCCR0B = (1<<CS02) | (1<<CS01);// | (1<<CS00);	// falling edge on T0/PD4
	TCNT0 = 0;
*/

	// Pin on crank events: PB0
	DDRB &= ~(1<<0);	// input on PB0
	PCMSK0 |= (1<<PCINT0);	// interrupt on PB0
	PCICR |= (1<<PCIE0); // interrupt on port B
	PINB_prev = PINB;	// read the values



	// Pin on cam events PD2
	DDRD &= ~(1<<2);	// input on PD2
	PCMSK2 |= (1<<PCINT18);	// interrupt on PD2
	PCICR |= (1<<PCIE2);	// interrupt enable on port D
	PIND_prev = PIND;


	// 16 bit timer 1 on 100us - main time measurement timer
//	TCCR1A = 0;	// normal mode
//	TIMSK1 = (1<<TOIE1) ;	// on overflow and compare
//	TCCR1B =  (1<<CS12);	// /256

	// Timer 0 to measure inter-teeth crank time
	TCCR0A =0;	// normal mode
	TCCR0B = (1<<CS02) | (1<<CS00);	// /256
	TCNT0 = 0;
	TIMSK0 = (1<<TOIE0);	// overflow

	// Timer 2 for execution of instantaneous tasks
	TCCR2A = 0;	// normal / ctc
	TCCR2B = 0;
//	TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);	// /1024

	sei();

	// Higher level components
	USART_Init();
	ADC_Init();



	while(1) {

		// Figure out the fires
		fireAt = 50;	// 90 degrees

		// Figure out the dwells from fires
		dwellAt = fireAt -15;	// 30 ~2ms



	}
}
