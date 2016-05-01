#include <avr/io.h>

#include "hack.h"

#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ADC.h"
#include "USART.h"
float req_FUEL = 45;	// 4.5ms

uint32_t ticks = 0;
uint16_t meas;
volatile uint16_t ticksPerDegree = 0;
volatile uint16_t nextEventAt = 0;
volatile uint16_t now = 0;
volatile uint16_t rpm = 0;
volatile uint8_t running = 0;
volatile uint16_t cycleCrankEvents = 0;

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
}








volatile uint8_t PINB_prev =0;
volatile uint32_t crankTicks = 0;
volatile uint32_t crankTime = 0;
volatile uint32_t crankTicksSum = 0;

#define CRANK_SIGNAL_AVG_WINDOW 24

int32_t errSum = 0;

volatile uint16_t degPeriod = 1;

void timer1_countTo(uint16_t countTo) {
	TCNT1H = 0;
	TCNT1L = 0;
	OCR1AH = countTo>>8;
	OCR1AL = countTo & 0xff;
	TIMSK1 |= (1<<OCIE1A);
}

volatile uint16_t cycleDegrees = 0;

void timer1_start() {
	TCCR1A = 0;	// normal mode
	// high BEFORE low on write
	TCNT1H = 0;
	TCNT1L = 0;
	// start the timer!
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);	// ctc, /64
}

volatile uint16_t coilDwellDegrees = 15;

volatile uint16_t injOpenDegrees = 1;
volatile uint16_t injOpen10thMs = 90;
volatile uint16_t advance = 13;

ISR(TIMER1_COMPA_vect) {
	PORTD ^= (1<<7);
	cycleDegrees++;
	timer1_countTo(degPeriod);
	timer1_start();

		// IGNITION
		if(cycleDegrees == 90-advance-coilDwellDegrees) {	// dwell 1 start
				PORTB |= (1<<2);
		}
		if(cycleDegrees == 90-advance) {		// fire 1!
				PORTB &= ~(1<<2);

		}

		if(cycleDegrees== 270-advance-coilDwellDegrees){	// dwell 2 start
			PORTB |= (1<<3);
		}
		if(cycleDegrees == 270-advance) {	// fire 2
			PORTB &= ~(1<<3);
		}

		if(cycleDegrees == 630-advance-coilDwellDegrees) {	// dwell 3
			PORTB |= (1<<4);
		}
		if(cycleDegrees ==  630-advance) {	// fire 3
			PORTB &= ~(1<<4);
		}

		if( cycleDegrees == 450-advance-coilDwellDegrees) {
			PORTB |= (1<<5);
		}
		if (cycleDegrees == 450-advance) {	// fire4
			PORTB &= ~(1<<5);
		}


		// INJECTION - ONLY 1 squirt!
		if(cycleDegrees == 90) {	// open em up!
			PORTB |= (1<<1);
		}
		if(cycleDegrees == 90+injOpenDegrees) {	//close' em'
			PORTB &= ~(1<<1);
		}



}





uint16_t reading = 0;
ISR(PCINT0_vect) {
	int32_t err = 0;

	// ONLY FALLING EDGE!
	if(( (PINB ^ PINB_prev) & (1<<0) ) && ( !(PINB & (1<<0)) )){

		reading = TCNT0 + (TCNT0H<<8);

		degPeriod = reading/30;


		coilDwellDegrees = 500/degPeriod;	// 500 corresponds to 2ms

		injOpenDegrees = (injOpen10thMs*25)/degPeriod;	// fuel pw in 10ms


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
		cycleCrankEvents++;
//		cycleDegrees = cycleCrankEvents*30;


		/*
		if(!started) {
			switch(cycleCrankEvents) {
				case 3:		// initially (4)
					PORTB |= (1<<2);
				break;

				case 15:	// dwell 2 start (initially 16)
					PORTB |= (1<<3);
				break;

				case 38:	// dwell 3 start (initially 40)
					PORTB |= (1<<4);
				break;

				case 27:	// initially (28)
					PORTB |= (1<<5);
				break;

				// 4 on 61.3 deg.
				// 5: 81.8 deg
				// 6: 102.2
				// 16:242.6
				// 17: 262.5
				// 18: 270.6
				// 40: 600.6, 598.2
				// 41: 621.2
				// 28: 421.3
				// 29: 439.7

				case 3:				// works fine!
					PORTB &= ~(1<<2);
				break;

				case 8:			// fine for now?
					PORTB &= ~(1<<3);
				break;


				case 20:
					PORTB &= ~(1<<4);
				break;

				case 14:			// works fine!
					PORTB &= ~(1<<5);
				break;
			}
		}
*/

		if((crankEvents %CRANK_SIGNAL_AVG_WINDOW) == 0) {
			crankTicks = crankTicksSum/CRANK_SIGNAL_AVG_WINDOW;
			crankTicksSum = 0;
		}


		//			crankTicks = TCNT0;
		crankTime = (crankTicks *100000)/15625;

	}
	PINB_prev = PINB;
}

ISR(TIMER2_COMPA_vect) {
//	PORTB &= ~(1<<5);
	TIMSK2 = 0;	// turn off the interrupt
	TCCR2B = 0;
}

void timer2_start(uint16_t countToMs) {
	TIMSK2 |= (1<<OCIE2A);
	OCR2A = countToMs*15;	// only for 1024
	TCNT2 = 0;
	TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);	// /1024
}




float enrich_MAP = 1;
float enrich_TPS = 1;
float enrich_ECT = 1;
// 250 ticks/ms
// ticks/degree
// deg/ms - 250/degPeriod



// Cam events
// Port D interrupt handler
volatile uint8_t PIND_prev = 0;
ISR(PCINT2_vect) {
	if( (PIND ^ PIND_prev) & (1<<2) ) {
		// FALLING edge on PD2 - cam signal
		if(!( PIND & (1<<2) )) {
			if(syncCrankEvents == 12) {
				crankEvents=0;	// restart cycle
				cycleDegrees = 0;
				cycleCrankEvents = 0;
			}
			syncCrankEvents = 0;
		}
	}
	PIND_prev= PIND;
}


float MAP_kpa = 0;
float TPS_perc = 0;
float TPS_min = 90;
float TPS_max = 900;

float ECT_tempF = 0;
float ECT_R = 0;
float ECT_raw = 0;

float IAT_raw = 0;
float IAT_tempC = 0;
float IAT_tempF = 0;
float IAT_R = 0;




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
	PORTD |= (1<<7);

	// MAP on PC0
	DDRC &= ~(1<<0);	// input on PC0

	// TPS on PC1
	DDRC &= ~(1<<1);	// input on PC1

	// ECT on PC2
	DDRC &= ~(1<<2);	// input on PC2

	// IAT on PC3
	DDRC &= ~(1<<3);	// input on PC3

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
	TCCR0B = (1<<CS01) | (1<<CS00);	// /64
	TCNT0 = 0;
	TIMSK0 = (1<<TOIE0);	// overflow


	timer1_countTo(degPeriod);
	timer1_start();


	// Timer 2 for execution of instantaneous tasks
	TCCR2A = 0;	// normal / ctc
	TCCR2B = 0;
//	TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);	// /1024

	sei();

	// Higher level components
	USART_Init();
	ADC_Init();


	uint16_t meas = 0;
	while(1) {
		meas = ADC_Convert(0);

//		  MAP_kpa = 36.06f*5.1f*(((float)ADC_Measurements[0])/1023.0f) - 0.2645f;
//		  enrich_MAP =  MAP_kpa/102.0f;

//		  injOpen10thMs = req_FUEL*5.0f*enrich_MAP;	// x10/2

			sprintf(out, "ADC meas %d, ", meas );
			USART_Print(out);

			sprintf(out, "MAP %.2f kpa, ", MAP_kpa );
			USART_Print(out);

			sprintf(out, "Enrich MAP %.2f, ", enrich_MAP );
			USART_Print(out);

			sprintf(out, "Inj_open deg %d, ", injOpenDegrees );
			USART_Print(out);

			sprintf(out, "Inj_pw %d /10ms, ", injOpen10thMs );
			USART_Print(out);

			USART_Print("\n");



		  /*

		// Reading the sensors

			IAT_raw = ADC_Convert( 3 );
		  IAT_R = 1000.0f*IAT_raw/(1024.0f-IAT_raw);
		  IAT_tempF = 1047.0*pow( IAT_R, -0.1693 ) - 222.7;
		  IAT_tempC = (IAT_tempF-32.0)/1.8;


		ECT_raw = ADC_Convert( 2 );
		  ECT_R = 1000.0f*ECT_raw/(1024.0f-ECT_raw);
		  ECT_tempF = 143.7f*exp(-0.0002789f*ECT_R) + 155.8f*exp( -0.003714f*ECT_R );


		  enrich_TPS = 0.005f*TPS_perc+0.5f;
		  enrich_ECT = ECT_tempF*-0.0102273 + 3.0f;

//		  inj_pw = 0.7f*req_FUEL* enrich_MAP *enrich_TPS*enrich_ECT ;

		  inj_pw = req_FUEL;

		  sprintf(out, "Reading: %d\n", reading);
		  USART_Print(out);

		  sprintf(out, "Deg period: %d\n", degPeriod);
		  USART_Print(out);


		sprintf(out, "ECT %.2f F, raw: %.2f, R: %.2f, ", ECT_tempF, ECT_raw, ECT_R );
		USART_Print(out);

		sprintf(out, "IAT %.2f F, raw: %.2f, R: %.2f, ", IAT_tempF, IAT_raw, IAT_R );
		USART_Print(out);




		sprintf(out, "TPS %.2f perc, ", TPS_perc );
		USART_Print(out);


		sprintf(out, "Enrich TPS %.2f, ", enrich_TPS );
		USART_Print(out);

		sprintf(out, "Enrich ECT %.2f, ", enrich_ECT );
		USART_Print(out);



		sprintf(out, "Crank ticks: %d\n",crankTicks );
		USART_Print(out);
*/

	}
}
