/**
 * This piece of software is used to capture the signature of the 
 * VRS signals on a 2001 Honda CBR 600 F4i's motor
 * VRS0 - crank
 * VRS1 - cam
 * Just capture first 190 events and then print them out on the serial
 */

#define VRS0_pin 2
#define VRS1_pin 3
#define CAPTURE_N 190

uint8_t vrs0_state[CAPTURE_N];
uint8_t vrs0_dt[CAPTURE_N];
uint8_t vrs1_state[CAPTURE_N];
uint8_t vrs1_dt[CAPTURE_N];
uint8_t vrs0_i=0;
uint8_t vrs1_i=0;
long vrs0_prevEvent = 0;
long vrs1_prevEvent = 0;

uint8_t ledstate = 0;
long vrs0_firstEvent = 0;
long vrs1_firstEvent = 0;

void bl() {
    ledstate = !ledstate;
  digitalWrite(13,ledstate);

}

void vrs0_event() {
  if(vrs0_i < CAPTURE_N) {

    if(vrs0_i >0) {
      vrs0_dt[vrs0_i] = millis()-vrs0_prevEvent;
      vrs0_prevEvent = millis();
    } else {
      vrs0_firstEvent = micros();
    }
    
    vrs0_state[vrs0_i] = digitalRead(VRS0_pin);
    vrs0_i++;
  }
  bl();
}

void vrs1_event() {
  if(vrs1_i < CAPTURE_N) {

    if(vrs1_i >0) {
      vrs1_dt[vrs1_i] = millis()-vrs1_prevEvent;
      vrs1_prevEvent = millis();
    } else {
        vrs1_firstEvent = micros();
    }

    
    vrs1_state[vrs1_i] = digitalRead(VRS1_pin);
    vrs1_i++;
  }
    bl();

}

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

pinMode(VRS0_pin, INPUT);
pinMode(VRS1_pin, INPUT);
pinMode(13, OUTPUT);
attachInterrupt(0, vrs0_event, CHANGE);
attachInterrupt(1, vrs1_event, CHANGE);
}

void loop() {

  if((vrs0_i ==CAPTURE_N ) || (vrs1_i ==CAPTURE_N )) {

Serial.println("VRS0 state: ");
  for(int i=0; i<CAPTURE_N; i++) {
    Serial.print(vrs0_state[i]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("VRS0 first event at [us]: ");
Serial.println(vrs0_firstEvent);

Serial.println("VRS0 dt [ms]: ");
  for(int i=0; i<CAPTURE_N; i++) {
    Serial.print(vrs0_dt[i]);
    Serial.print(", ");
  }
  Serial.println();
Serial.println("VRS1 state: ");
  for(int i=0; i<CAPTURE_N; i++) {
    Serial.print(vrs1_state[i]);
    Serial.print(", ");
  }
  Serial.println();
Serial.print("VRS1 first event at [us]: ");
Serial.println(vrs1_firstEvent);
Serial.println("VRS1 dt [ms]: ");
  for(int i=0; i<CAPTURE_N; i++) {
    Serial.print(vrs1_dt[i]);
    Serial.print(", ");
  }
  Serial.println();
    
    delay(1000);
  }
}
