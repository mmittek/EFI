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


void coilsOff() {
	// SET ALL THE COILS TO OFF
	PORTB &= ~(1<<5);
	PORTB &= ~(1<<4);
	PORTB &= ~(1<<3);
	PORTB &= ~(1<<2);
}

void timer1_stop() {
	TCCR1B = 0;
	TIMSK1 = 0;
}


void injOff() {
	PORTB &= (1<<1);	// inj off
}

void allOff() {
	timer1_stop();
	coilsOff();
	injOff();
}

volatile uint8_t TCNT0H = 0;
volatile uint8_t started = 0;
ISR(TIMER0_OVF_vect) {
	TCNT0H++;
	started = 0;
//	running = 0;
//	allOff();
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
volatile uint32_t crankTicksSum = 0;

#define CRANK_SIGNAL_AVG_WINDOW 24

int32_t errSum = 0;

ISR(PCINT0_vect) {
	uint16_t reading = 0;
	int32_t err = 0;

	// Change on PB0
	if( (PINB ^ PINB_prev) & (1<<0) ) {

		reading = TCNT0 + (TCNT0H<<8);
		TCNT0 = 0;
		TCNT0H = 0;
		crankTicksSum += reading;

		err = reading - crankTicks;
		errSum += err;

		// PID?
		//crankTicks += err/64 + errSum/64;


		syncCrankEvents++;
		running = 1;
		crankEvents++;

		if(!started) {

			switch(crankEvents) {
				case 4:
					PORTB |= (1<<2);	// dwell stop FIRE
				break;
				case 16:
					PORTB |= (1<<3);	// dwell stop FIRE
				break;
				case 40:
					PORTB |= (1<<4);	// dwell stop FIRE
				break;
				case 28:
					PORTB |= (1<<5);	// dwell stop FIRE
				break;



				case 5:
					PORTB &= ~(1<<2);	// dwell stop FIRE
				break;
				case 17:
					PORTB &= ~(1<<3);	// dwell stop FIRE
				break;
				case 41:
					PORTB &= ~(1<<4);	// dwell stop FIRE
				break;
				case 29:
					PORTB &= ~(1<<5);	// dwell stop FIRE
				break;
			}
	//		myEvents[0].at = (crankTicks*6);	// 90 degrees
	//		myEvents[1].at = (crankTicks*18);	// 270 degrees
	//		myEvents[2].at = (crankTicks*42);	// 630 degrees
	//		myEvents[3].at = (crankTicks*30);	// 450 degrees
		}


		if((crankEvents %CRANK_SIGNAL_AVG_WINDOW) == 0) {
			crankTicks = crankTicksSum/CRANK_SIGNAL_AVG_WINDOW;
			crankTicksSum = 0;
		}
		// ARBITRARILY
		if(crankTicks <= 10) {
			//started = 1;
		}


		//			crankTicks = TCNT0;
		crankTime = (crankTicks *100000)/15625;

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



#define COIL0_DWELL 	1
#define COIL0_FIRE 		2
#define COIL1_DWELL		3
#define COIL1_FIRE		4
#define COIL2_DWELL		5
#define COIL2_FIRE		6
#define COIL3_DWELL		7
#define COIL3_FIRE		8
#define INJ_OPEN		9
#define INJ_CLOSE		10
#define DONE 			11




typedef struct {
	uint16_t at;
	uint8_t action;
	uint8_t done;
}myEvent_t;

#define NUMEVENTS 17
myEvent_t myEvents[NUMEVENTS];
volatile uint8_t currentEventIdx = 0;



uint8_t findFirstEvent() {
	uint16_t min = 65535;
	uint8_t result = 255;
	for(int i=0; i<NUMEVENTS; i++) {
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
	for(int i=0; i<NUMEVENTS; i++) {
		myEvents[i].done = 0;
	}
}

void executeEventAction(uint8_t action) {


	if(started) {
		switch(action) {
			case COIL0_DWELL:
				PORTB |= (1<<2);	// dwell start
			break;
			case COIL0_FIRE:
				PORTB &= ~(1<<2);	// dwell stop FIRE
			break;
			case COIL1_DWELL:
				PORTB |= (1<<3);	// dwell start
			break;
			case COIL1_FIRE:
				PORTB &= ~(1<<3);	// dwell start
			break;
			case COIL2_DWELL:
				PORTB |= (1<<4);	// dwell start
			break;
			case COIL2_FIRE:
				PORTB &= ~(1<<4);	// dwell start
			break;
			case COIL3_DWELL:
				PORTB |= (1<<5);	// dwell start
			break;
			case COIL3_FIRE:
				PORTB &= ~(1<<5);	// dwell start
			break;
		}
	}


	switch(action) {

		case INJ_OPEN:
			PORTB |= (1<<1);	// PB1
		break;

		case INJ_CLOSE:
			PORTB &= ~(1<<1);	// PB1
		break;

		case DONE:
			allOff();
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
//	TIMSK1 = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);	// ctc, /1024
}


uint16_t coilDwellTimeTicks = 45;	// 45 ~ 3ms
uint16_t req_FUEL = 2;
uint16_t enrich_MAP = 100;


void reschedule() {
	myEvents[0].at = (crankTicks*6);	// 90 degrees
	myEvents[1].at = (crankTicks*18);	// 270 degrees
	myEvents[2].at = (crankTicks*42);	// 630 degrees
	myEvents[3].at = (crankTicks*30);	// 450 degrees

	// DONE!
	myEvents[NUMEVENTS-1].at = (crankTicks*48);	// 720 degrees

	// 13 degrees advance
	for(int i=0; i<4; i++) {
		myEvents[i].at -= (13*crankTicks)/15;
	}

	for(int i=4; i<8; i++) {
		myEvents[i].at = myEvents[i-4].at - coilDwellTimeTicks;
	}

	// INJECTION PULSE 8-11 ON
	// INJECTOR PULSE 12-15 OFF

	for(int i=8; i<=11; i++) {
		myEvents[i].at = myEvents[i-8].at+1;	// copy the ignition
		myEvents[i+4].at = myEvents[i].at + req_FUEL *15;	// add 5ms
	}

}

// Cam events
// Port D interrupt handler
volatile uint8_t PIND_prev = 0;
ISR(PCINT2_vect) {
	if( (PIND ^ PIND_prev) & (1<<2) ) {
		// Falling edge on PD2 - cam signal
		if(!( PIND & (1<<2) )) {
			if(syncCrankEvents == 24) {
				crankEvents=0;	// restart cycle
				timer1_stop();
				timer1_start();
				reschedule();
				undoAllEvents();
				currentEventIdx = findFirstEvent();
				timer1_countTo( myEvents[currentEventIdx].at );
			}
			crankEventsArr[crankEventsArri] = syncCrankEvents;
			crankEventsArri = (crankEventsArri+1)%5;
			syncCrankEvents = 0;
		}
	}
	PIND_prev= PIND;
}


float MAP_volts = 0;
int MAP_kpa = 0;
uint16_t MAP_raw;

int main() {
	char out[32];



	cli();

	// COIL PINS ON PORT B
	DDRB |= (1<<5);
	DDRB |= (1<<4);
	DDRB |= (1<<3);
	DDRB |= (1<<2);
	PORTB &= ~(1<<5);	// PB5
	PORTB &= ~(1<<4);	// PB4
	PORTB &= ~(1<<3);	// PB3
	PORTB &= ~(1<<2);	// PB2

	// PB1 - INJ
	DDRB |= (1<<1);	// PB1
	PORTB &= ~(1<<1);

	// PD7 diode output
	DDRD |= (1<<7);	// PD7
	PORTD &= ~(1<<7);

	// MAP on PC0
	DDRC &= ~(1<<0);	// input on PC0

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




	myEvents[0].action = COIL0_FIRE;
	myEvents[1].action = COIL1_FIRE;
	myEvents[2].action = COIL2_FIRE;
	myEvents[3].action = COIL3_FIRE;

	myEvents[4].action = COIL0_DWELL;
	myEvents[5].action = COIL1_DWELL;
	myEvents[6].action = COIL2_DWELL;
	myEvents[7].action = COIL3_DWELL;

	myEvents[8].action = INJ_OPEN;
	myEvents[9].action = INJ_OPEN;
	myEvents[10].action = INJ_OPEN;
	myEvents[11].action = INJ_OPEN;

	myEvents[12].action = INJ_CLOSE;
	myEvents[13].action = INJ_CLOSE;
	myEvents[14].action = INJ_CLOSE;
	myEvents[15].action = INJ_CLOSE;

	myEvents[NUMEVENTS-1].action = DONE;

	while(1) {

		if(started) {
			PORTD &= ~(1<<7);
		} else {
			PORTD |= (1<<7);
		}

		MAP_raw = ADC_Convert(0);	// ADC0
		  MAP_volts = 5.1f*((float)MAP_raw)/1023.0f;
		  //MAP_kpa =  8+ 40.0f*5.1f*((float)MAP_raw)/1023.0f;
		  MAP_kpa = 36.06f*5.1f*(((float)MAP_raw)/1023.0f) - 0.2645f;

		 // enrich_MAP = 100*(105/(105-MAP_kpa));

		//reschedule();


		sprintf(out, "MAP raw: %d, %d kpa\n",MAP_raw, MAP_kpa );
		USART_Print(out);

		sprintf(out, "Crank ticks: %d\n",crankTicks );
		USART_Print(out);


	}
}
