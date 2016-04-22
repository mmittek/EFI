
// TRIGGER ON THE FALLING EDGE OF THE CRANK
#include <avr/io.h> 
#include <avr/interrupt.h>

const int VRS0_pin = 2;
const int VRS1_pin = 3;
const int KNOB_pin = A0;

volatile unsigned int VRS0_events = 0;
volatile unsigned int VRS1_events = 0;

int tickPerEvent[128];
volatile uint8_t INJ_time = 0;
volatile long ECU_cycleTimeMs = 0;
//volatile long VRS0_lastEventAtMs = 0;
volatile long now = 0;
int VRS0_degreesPerTooth = 30;
long ECU_msPerRotation = 0;
long ECU_lastSyncAtMs = 0;
long ECU_lastSyncAtUs = 0;
long ECU_now = 0;
long ECU_nowUs = 0;
long ECU_cycleStartedAtMs = 0;

typedef enum {
  STATE_NONE = 0,
  STATE_BDC,
  STATE_REF,
  STATE_103BTDC,
  STATE_ERROR,
}state_t;

state_t state = STATE_NONE;
volatile uint16_t ECU_currentDegree = 0;



uint16_t VRS0_eventTimesMs[12];
uint8_t VRS0_currentEvent = 0;
volatile long VRS0_lastEventAtMs = 0;

void VRS0_isr() {
  VRS0_events++;
  VRS0_currentEvent = (VRS0_currentEvent+1)%12;
  long now = millis();
  VRS0_eventTimesMs[VRS0_currentEvent] = now-VRS0_lastEventAtMs;
  VRS0_lastEventAtMs = now;
}

volatile int ECU_cycleDone = 1;



void flashOn() {
  digitalWrite(12, 1);

}

void flashOff() {
  digitalWrite(12, 0);
}

void VRS1_isr() {
  VRS1_events++;
  ECU_now = millis();
  
  switch(VRS0_events) {

    case 2:
      state = STATE_REF;
    break;

    case 10:
      state = STATE_BDC;
    break;

    case 12:
      state = STATE_103BTDC;
      ECU_cycleStartedAtMs = ECU_now;
      ECU_msPerRotation = (ECU_now - ECU_lastSyncAtMs)/2;
      ECU_lastSyncAtMs = ECU_now;
      ECU_cycleTimeMs = 0;
    break;
    
    default:
      state = STATE_ERROR;
  }
  VRS0_events = 0;  
}

int ledstate = 0;

const int ECU_coilDwellMs = 2;
const int ECU_injectorDeadTimeMs = 1;
// 1 2 4 3
const int16_t ECU_syncBTDCdeg = 103;

//int16_t ECU_ignitionTiming[] = {90, 270, 630, 450};
int16_t ECU_ignitionTiming[] = {90, 270, 630, 450};

int16_t ECU_injectionTiming[] = {630, 450, 270, 90};  // whatever
uint8_t ECU_coilPins[] = {9, 10, 11, 12};
uint8_t ECU_injectorPins[] = {5,6,7,8};
const int ECU_cylinders = 4;

volatile uint16_t ECU_coilDwellStartMs[4];
volatile uint16_t ECU_coilDwellStopMs[4];
volatile uint16_t ECU_injectorOpenMs[4];
volatile uint16_t ECU_injectorCloseMs[4];

ISR (TIMER2_COMPA_vect) {
  //long diff;
  uint8_t i;
  ECU_cycleTimeMs++;
  
  
  for(i=0; i<ECU_cylinders; i++) {
    // Coil dwelling
//    if(ECU_cycleTimeMs == (( ECU_ignitionTiming[i] *ECU_msPerRotation)/360)-ECU_coilDwellMs)  {
    if(ECU_cycleTimeMs == ECU_coilDwellStartMs[i])  {
      digitalWrite(ECU_coilPins[i],1);
//      ECU_ignitionStates[i] = 1;
    }
    
    // Coil dwell stop - discharge BANG!
//    if(ECU_cycleTimeMs == (( ECU_ignitionTiming[i] *ECU_msPerRotation)/360))  {
    if(ECU_cycleTimeMs == ECU_coilDwellStopMs[i] )  {
      digitalWrite(ECU_coilPins[i],0);
//            ECU_ignitionStates[i] = 0;

    }
    
    // Opening injectors
    if(ECU_cycleTimeMs == ECU_injectorOpenMs[i])  {
      digitalWrite(ECU_injectorPins[i],1);
//      ECU_injectorStates[i] = 1;
    }

    // closing injectors
//    if(ECU_cycleTimeMs == (( ECU_injectionTiming[i] *ECU_msPerRotation)/360)+INJ_time)  {
    if(ECU_cycleTimeMs ==ECU_injectorCloseMs[i])  {
      digitalWrite(ECU_injectorPins[i],0);
//      ECU_injectorStates[i] = 0;
    }

  //  digitalWrite(ECU_coilPins[i],ECU_ignitionStates[i]);
   // digitalWrite(ECU_injectorPins[i],ECU_injectorStates[i]);
  }
  


  
  
}

void setup() {
  Serial.begin(9600);
  
  pinMode(VRS0_pin, INPUT);
  pinMode(VRS1_pin, INPUT);

  pinMode(KNOB_pin, INPUT);
  
  for(int i=0; i<ECU_cylinders; i++) {
    pinMode( ECU_coilPins[i], OUTPUT );
    digitalWrite( ECU_coilPins[i], 0 );

    pinMode( ECU_injectorPins[i], OUTPUT );
    digitalWrite( ECU_injectorPins[i], 0 );

  }
  
  attachInterrupt( 0, VRS0_isr, FALLING );
  attachInterrupt( 1, VRS1_isr, RISING);
  tickPerEvent[0] = 0;
  
  
    OCR2A = 1;
    TCCR2A |= (1 << WGM21);
    TIMSK2 |= (1 << OCIE2A);
    TCCR2B |= 4;

  
}


void flash() {
digitalWrite(12, 1);
delayMicroseconds(500);
digitalWrite(12, 0);
}

char line[128];
int prev_state = 0;



volatile long ECU_currentCycleTimeMs = 0;


void loop() {
  
  uint16_t KNOB_value = analogRead(KNOB_pin);  
  INJ_time = 30*KNOB_value/1024;
  
  for(int i=0; i<4; i++) {
   ECU_coilDwellStartMs[i] = (( ECU_ignitionTiming[i] *ECU_msPerRotation)/360)-ECU_coilDwellMs;
   ECU_coilDwellStopMs[i] = (( ECU_ignitionTiming[i] *ECU_msPerRotation)/360);
   ECU_injectorOpenMs[i] = (( ECU_injectionTiming[i] *ECU_msPerRotation)/360)-ECU_injectorDeadTimeMs;
   ECU_injectorCloseMs[i] = (( ECU_injectionTiming[i] *ECU_msPerRotation)/360)+INJ_time;
  }
  /*
  Serial.print("Injector time: ");
  Serial.print(INJ_time);
  Serial.println(" ms");
  
  Serial.println("Event times: ");
  for(int i=0; i<12; i++) {
    Serial.print( VRS0_eventTimesMs[i] );
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("Ms per rotation: ");
  Serial.println(ECU_msPerRotation);
  */
 // delay(1000);
  
}

