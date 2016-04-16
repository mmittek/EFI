
// TRIGGER ON THE FALLING EDGE OF THE CRANK
#include <avr/io.h> 
#include <avr/interrupt.h>

const int VRS0_pin = 2;
const int VRS1_pin = 3;

const int IND0_pin = 11;
int IND0_state = 0;
const int IND1_pin = 12;
int IND1_state = 0;

volatile unsigned int VRS0_events = 0;
volatile unsigned int VRS1_events = 0;

int tickPerEvent[128];

volatile long ECU_cycleTimeMs = 0;
volatile long VRS0_lastEventAtMs = 0;
volatile long now = 0;
int VRS0_degreesPerTooth = 30;
long ECU_msPerRotation = 0;
long ECU_lastSyncAtMs = 0;
long ECU_now = 0;
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


typedef void (*ECU_task_t)(void);
ECU_task_t ECU_tasks[720];

void ECU_clearTasks() {
memset(ECU_tasks, 0, sizeof(ECU_tasks));
}


void VRS0_isr() {
  VRS0_events++;
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
  switch(VRS0_events) {

    case 2:
      state = STATE_REF;
    break;

    case 10:
      state = STATE_BDC;
    break;

    case 12:
      state = STATE_103BTDC;
      ECU_now = millis();
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

ISR (TIMER2_COMPA_vect) {
  //long diff;
  ECU_cycleTimeMs++;
  ledstate = !ledstate;
  digitalWrite(11,ledstate);
  
  
  if(ECU_cycleTimeMs == ((90*ECU_msPerRotation)/360)-2) {
    digitalWrite(12,1);
  }

  if(ECU_cycleTimeMs == (90*ECU_msPerRotation)/360) {
    digitalWrite(12,0);
  }

  
  if(ECU_cycleTimeMs == ((270*ECU_msPerRotation)/360)-2) {
    digitalWrite(12,1);
  }

  if(ECU_cycleTimeMs == (270*ECU_msPerRotation)/360) {
    digitalWrite(12,0);
  }

  if(ECU_cycleTimeMs == ((450*ECU_msPerRotation)/360)-2) {
    digitalWrite(12,1);
  }

  if(ECU_cycleTimeMs == (450*ECU_msPerRotation)/360) {
    digitalWrite(12,0);
  }

  if(ECU_cycleTimeMs == ((630*ECU_msPerRotation)/360)-2) {
    digitalWrite(12,1);
  }

  if(ECU_cycleTimeMs == (630*ECU_msPerRotation)/360) {
    digitalWrite(12,0);
  }
  
  
}

void setup() {
  Serial.begin(9600);
  
  pinMode(VRS0_pin, INPUT);
  pinMode(VRS1_pin, INPUT);

pinMode(11, OUTPUT);

  pinMode(IND0_pin, OUTPUT);
  digitalWrite(IND0_pin, 0);
  pinMode(IND1_pin, OUTPUT);
  digitalWrite(IND1_pin, 0);
  
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

  /*
  if(state != prev_state) {
    
    switch(state) {
      case STATE_103BTDC:

      //delay(( (90) *ECU_msPerRotation)/360);
      //flash();
        Serial.println("STATE_103BTDC");
  Serial.println(ECU_msPerRotation);
  Serial.println( 60000/ECU_msPerRotation );

   
      break;
      case STATE_REF:
//       flash();
        Serial.println("REF");
      break;
      case STATE_BDC:
        Serial.println("BDC");
      break;
      case STATE_ERROR:
        Serial.println("ERROR!");
      break;
    }
    
    
    
    prev_state = state;
  }
  */
/*
  sprintf(line, "VRS0: %d, VRS1: %d", VRS0_events, VRS1_events);
  Serial.println(line);
  
  for (int i=0; i<32; i++) {
    Serial.println( tickPerEvent[i] );
  }
  
  delay(1000);
//  digitalWrite(IND0_pin, IND0_state);
//  digitalWrite(IND1_pin, IND1_state);
  */
  

}

