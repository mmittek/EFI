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




typedef struct {
	uint16_t at;
	uint8_t action;
	uint8_t done;
}myEvent_t;

myEvent_t myEvents[8];
volatile uint8_t currentEventIdx = 0;

uint8_t findFirstEvent() {
	uint16_t min = 65535;
	uint8_t result = 255;
	for(int i=0; i<8; i++) {
		if(myEvents[i].done == 0) {
			if( myEvents[i].at < min ) {
				result = i;
				min = myEvents[i].at;
			}
		}
	}
	return result;
}

void undoAllEvents() {
	for(int i=0; i<8; i++) {
		myEvents[i].done = 0;
	}
}

void executeEventAction(uint8_t action) {
	switch(action) {
		case COIL0_DWELL:
			PORTB |= (1<<5);	// dwell start
		break;
		case COIL0_FIRE:
			PORTB &= ~(1<<5);	// dwell start
		break;
		case COIL1_DWELL:
			PORTB |= (1<<5);	// dwell start
		break;
		case COIL1_FIRE:
			PORTB &= ~(1<<5);	// dwell start
		break;
		case COIL2_DWELL:
			PORTB |= (1<<5);	// dwell start
		break;
		case COIL2_FIRE:
			PORTB &= ~(1<<5);	// dwell start
		break;
		case COIL3_DWELL:
			PORTB |= (1<<5);	// dwell start
		break;
		case COIL3_FIRE:
			PORTB &= ~(1<<5);	// dwell start
		break;
	}
}


ISR(TIMER1_COMPA_vect) {
	uint16_t dt;
	uint8_t nextEvent;
	myEvents[currentEventIdx].done = 1;
	nextEvent = findFirstEvent();
	dt = myEvents[nextEvent].at-myEvents[currentEventIdx].at;
	timer1_countTo( dt );
	executeEventAction(myEvents[currentEventIdx].action);
	currentEventIdx = nextEvent;
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
				undoAllEvents();
				// Count to the first event
				currentEventIdx = findFirstEvent();
				timer1_countTo( myEvents[currentEventIdx].at );
//				timer1_countTo( COIL0_dwellAt );	// another tooth for some reason
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

		myEvents[0].action = COIL0_FIRE;
		myEvents[1].action = COIL1_FIRE;
		myEvents[2].action = COIL2_FIRE;
		myEvents[3].action = COIL3_FIRE;

		myEvents[4].action = COIL0_DWELL;
		myEvents[5].action = COIL1_DWELL;
		myEvents[6].action = COIL2_DWELL;
		myEvents[7].action = COIL3_DWELL;

		myEvents[0].at = (crankTicks*3);	// 90 degrees
		myEvents[1].at = (crankTicks*9);	// 270 degrees
		myEvents[2].at = (crankTicks*21);	// 630 degrees
		myEvents[3].at = (crankTicks*15);	// 450 degrees

		for(int i=4; i<8; i++) {
			myEvents[i].at = myEvents[i-4].at - coilDwellTimeTicks;
		}



		sprintf(out, "Crank ticks: %d\n",crankTicks );
		USART_Print(out);

		USART_Print("Crank events: \n");
		for(int i=0; i<5; i++) {
			sprintf(out, "%d\n",crankEventsArr[i] );
			USART_Print(out);

		}

	}
}
