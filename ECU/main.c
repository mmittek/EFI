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


volatile uint16_t syncCrankEvents = 0;
volatile uint16_t crankEvents = 0;

typedef enum {
	SYNC = 0,
	COIL0_DWELL,
	COIL0_FIRE,
	COIL1_DWELL,
	COIL1_FIRE,
	COIL2_DWELL,
	COIL2_FIRE,
	COIL3_DWELL,
	COIL3_FIRE,
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

		// falling edge only!
		if(!(PINB & (1<<0))) {
			crankEvents++;
			syncCrankEvents++;
			running = 1;
			crankTicks = TCNT0;
			TCNT0 = 0;
			crankTime = (crankTicks *100000)/15625;
		}
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

void timer1_countTo(uint16_t countTo) {
	TCNT1H = 0;
	TCNT1L = 0;
	OCR1AH = countTo>>8;
	OCR1AL = countTo & 0xff;
	TIMSK1 |= (1<<OCIE1A);
}

void timer1_stop() {
	TCCR1B = 0;
	TIMSK1 = 0;
}


#define COIL0_DWELL 	1
#define COIL0_FIRE 		2
#define COIL1_DWELL		3
#define COIL1_FIRE		4
#define COIL2_DWELL		5
#define COIL2_FIRE		6
#define COIL3_DWELL		7
#define COIL3_FIRE		8
volatile uint16_t now;
volatile uint16_t COIL0_dwellAt=0;
volatile uint16_t COIL0_fireAt=0;
volatile uint16_t COIL1_dwellAt=0;
volatile uint16_t COIL1_fireAt=0;
volatile uint16_t COIL2_dwellAt=0;
volatile uint16_t COIL2_fireAt=0;
volatile uint16_t COIL3_dwellAt=0;
volatile uint16_t COIL3_fireAt=0;

ISR(TIMER1_COMPA_vect) {
	now =OCR1AL;
	now |= OCR1AH<<8;

	currentState++;
	if(currentState == COIL0_DWELL) {
//	if(now == COIL0_fireAt) {
		PORTB |= (1<<5);	// dwell start
		timer1_countTo( COIL0_fireAt-COIL0_dwellAt );
		return;
	}
	if(currentState ==COIL0_FIRE) {
		PORTB &= ~(1<<5);	// dwell stop - FIRE!
		timer1_countTo( COIL1_dwellAt-COIL0_fireAt );
		return;
	}
	if(currentState == COIL1_DWELL) {
		PORTB |= (1<<5);	// dwell start
		timer1_countTo( COIL1_fireAt-COIL1_dwellAt );
		return;
	}
	if(currentState == COIL1_FIRE) {
		PORTB &= ~(1<<5);	// FIRE!
		timer1_countTo( COIL2_dwellAt-COIL1_fireAt );
		return;
	}
	if(currentState == COIL2_DWELL) {
		PORTB |= (1<<5);	// dwell start
		timer1_countTo( COIL2_fireAt-COIL2_dwellAt );
		return;
	}
	if(currentState == COIL2_FIRE) {
		PORTB &= ~(1<<5);	// dwell stop - FIRE!
		timer1_countTo( COIL3_fireAt-COIL2_fireAt );
	}
	if(currentState == COIL3_DWELL) {
		PORTB |= (1<<5);	// dwell start
		timer1_countTo( COIL3_fireAt-COIL3_dwellAt );
		return;
	}
	if(currentState == COIL3_FIRE) {
		PORTB &= ~(1<<5);	// dwell stop - FIRE!
		timer1_stop();
	}
}




void timer1_start() {
	TCCR1A = 0;	// normal mode
	// high BEFORE low on write
	TCNT1H = 0;
	TCNT1L = 0;
	// start the timer!
	TIMSK1 = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);	// ctc, /1024
}

// Cam events
// Port D interrupt handler
volatile uint8_t PIND_prev = 0;
ISR(PCINT2_vect) {
	if( (PIND ^ PIND_prev) & (1<<2) ) {

		// Falling edge on PD2 - cam signal
		if(!( PIND & (1<<2) )) {
			if(syncCrankEvents == 12) {
				// SYNC JUST STARTED xD
				crankEvents = 0;

				currentState = 0;
				timer1_stop();
				// Count to the first event
				timer1_countTo( COIL0_dwellAt );	// another tooth for some reason
				timer1_start();


			}
			crankEventsArr[crankEventsArri] = syncCrankEvents;
			crankEventsArri = (crankEventsArri+1)%5;
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


	uint16_t coilDwellTimeTicks = 7;
	while(1) {

		// Figure out the ignition TIMING for FIRE
		for(int i=0; i<4; i++) {
//			uint16_t ignitionAngles[] = {90, 270, 630, 450};
		}

//		cli();
			COIL0_fireAt = (crankTicks*3);	// 90 degrees
			COIL0_dwellAt = COIL0_fireAt-coilDwellTimeTicks;	// 30 ~2ms
			COIL1_fireAt = (crankTicks*9);	// 270 degrees
			COIL1_dwellAt = COIL1_fireAt-coilDwellTimeTicks;	// 30 ~2ms
			COIL2_fireAt = (crankTicks*15);	// 450 degrees
			COIL2_dwellAt = COIL2_fireAt-coilDwellTimeTicks;	// 30 ~2ms
			COIL3_fireAt = (crankTicks*21);	// 630 degrees
			COIL3_dwellAt = COIL3_fireAt-coilDwellTimeTicks;	// 30 ~2ms
//		sei();

		sprintf(out, "Crank ticks: %d\n",crankTicks );
		USART_Print(out);

		USART_Print("Crank events: \n");
		for(int i=0; i<5; i++) {
			sprintf(out, "%d\n",crankEventsArr[i] );
			USART_Print(out);

		}

	}
}
