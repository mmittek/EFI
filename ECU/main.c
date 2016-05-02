#include <avr/io.h>

#include "hack.h"

#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ADC.h"
#include "USART.h"

// TYPES
typedef enum {
	STATE_STOP = 0,
	STATE_CRANKING,
	STATE_RUNNING,
}runstate_t;

// CONFIG
#define camMarkerAngleDeg -90
#define CamMarkerCrankEvents 12
#define CrankEventsPerCycle 24
#define RpmNum 312500				// what needs to be divided by the crank interval to get the actual value of rpms
#define CrankingRpmTh 600

// VARIABLES
volatile uint8_t TCNT0H = 0;	// 8 bit value to extend timer0's range
volatile uint16_t cycleCrankEvents = 0;
volatile uint8_t syncCrankEvents = 0;
volatile int16_t cycleAngleDeg = 0;
uint16_t crankCurve[CrankEventsPerCycle];
volatile uint32_t errors = 0;
volatile uint32_t rpm = 0;
volatile runstate_t runstate = STATE_STOP;


#define ERROR_TOO_MANY_CRANK_EVENTS_PER_CYCLE 1

// Crank events
ISR(INT0_vect) {
	uint16_t crankEventInterval = TCNT0 | (TCNT0H<<8);
	if(cycleCrankEvents < CrankEventsPerCycle) {
		crankCurve[cycleCrankEvents] = crankEventInterval;
	} else {
		errors |= ERROR_TOO_MANY_CRANK_EVENTS_PER_CYCLE;
	}
	rpm = RpmNum/crankEventInterval;

	if(rpm < CrankingRpmTh) {
		runstate = STATE_CRANKING;
	} else {
		runstate = STATE_RUNNING;
	}

	cycleCrankEvents++;
	syncCrankEvents++;
	TCNT0H =0;
	TCNT0 = 0;
}

// Cam events
ISR(INT1_vect) {
	switch(syncCrankEvents) {
		case CamMarkerCrankEvents:
			cycleCrankEvents = 0;				// reset the cycle crank event counter
			cycleAngleDeg = camMarkerAngleDeg;	//
			TCNT0 = 0;
			TCNT0H = 0;
		break;
	}
	syncCrankEvents = 0;
}


ISR(TIMER0_OVF_vect) {
	TCNT0H++;

	// More than a second - come on!
	if(TCNT0H > 0xF4) {
		runstate = STATE_STOP;
		rpm = 0;
	}
}

int main() {

	USART_Init();
	char out[512];

	// SETUP
	cli();
	DDRB |= (1<<5);	// PB5 / 13 output
	PORTB &=~(1<<5);


	DDRD &=~(1<<2);	// Input on PD2
	DDRD &=~(1<<3);	// Input on PD3

	EICRA = (1<<ISC11);	// falling edge on INT1
	EICRA |= (1<<ISC01);	// falling edge on INT0
	EIMSK = (1<<INT0) | (1<<INT1);	// both interrupts on
	PCMSK0 = (1<<PCINT0) | (1<<PCINT1);


	TCCR0A = 0;	// Normal mode
	TCNT0 = 0;
	TCCR0B = (1<<CS02);// / 256
	TIMSK0 = (1<<TOIE0);

	sei();


	while(1) {
			sprintf(out, "RPM: %ld\n", rpm);
			USART_Print(out);

			switch(runstate) {
				case STATE_STOP:
					USART_Print("STATE: STOP\n");
				break;
				case STATE_CRANKING:
					USART_Print("STATE: CRANKING\n");
				break;
				case STATE_RUNNING:
					USART_Print("STATE: RUNNING\n");
				break;
			}

			USART_Print("Crank curve: \n");
			for(int i=0; i<CrankEventsPerCycle; i++) {
				sprintf(out, "%d, ", crankCurve[i]);
				USART_Print(out);
			}
			USART_Print("\n");
	}
	return 0;
}
