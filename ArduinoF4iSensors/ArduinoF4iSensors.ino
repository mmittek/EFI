

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to
const int LED_pin = 13; // Analog output pin that the LED is attached to

const int MAP_pin = A3;
int MAP_raw = 0;
float MAP_volts = 0;
int MAP_kpa = 0;

const int TPS_pin = A2;
int TPS_raw = 0;
int TPS_min = 90;
int TPS_max = 900;
int TPS_val = 0;

int ECT_pin = A1;
float ECT_raw = 0;
float ECT_R = 0;
float ECT_volts = 0;
float ECT_tempF = 0;
float ECT_tempC = 0;

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

float ratio;
float R1 = 997;
float RS = 0;
float tempF = 0;
float tempC = 0;

int LED_state = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  pinMode( analogInPin, INPUT );
  pinMode( TPS_pin, INPUT );
  pinMode( MAP_pin, INPUT );
  pinMode(ECT_pin, INPUT);
  
  pinMode(LED_pin, OUTPUT);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);            
  ratio = ((float)sensorValue)/1023.0;
  RS = R1*sensorValue/(1023-sensorValue);
  tempF = 1047.0*pow( RS, -0.1693 ) - 222.7;
  tempC = (tempF-32.0)/1.8;
  // map it to the range of the analog out:
  outputValue = sensorValue;//map(sensorValue, 0, 1023, 0, 255);  

  ECT_raw = analogRead( ECT_pin );
  ECT_R = R1*ECT_raw/(1023.0f-ECT_raw);
  ECT_tempF = 143.7f*exp(-0.0002789f*ECT_R) + 155.8f*exp( -0.003714f*ECT_R );
  
  TPS_raw = analogRead(TPS_pin);
  TPS_val = 1000.0f*((float)(TPS_raw-TPS_min))/((float)(TPS_max-TPS_min));
  
  MAP_raw = analogRead(MAP_pin);
  MAP_volts = 5.1f*((float)MAP_raw)/1023.0f;
  MAP_kpa =  8+ 40.0f*5.1f*((float)MAP_raw)/1023.0f;
  /*
  if(TPS_raw < TPS_min) TPS_min = TPS_raw;
  if(TPS_raw > TPS_max) TPS_max = TPS_raw;
*/


  // print the results to the serial monitor:
/*
  Serial.print("sensor = " );                       
  Serial.print(sensorValue);      
  Serial.print("\t output = ");  
  Serial.print("Ratio: ");
  Serial.print(ratio);
Serial.print(", RS: "); 
Serial.print(RS);
Serial.print(", Temp [F]: ");
Serial.print(tempF);
*/
Serial.print("TPS: ");
Serial.print(TPS_val);

Serial.print(", MAP: ");
Serial.print(MAP_raw);
Serial.print(", ");
Serial.print(MAP_kpa);
Serial.print("kpa, ");
Serial.print(MAP_volts);
Serial.print("V");

Serial.print(", IAT_R [ohm]: ");
Serial.print(RS);
Serial.print(", IAT [C]: ");
Serial.print(tempC);


Serial.print(", ECT_R [ohm]: ");
Serial.print(ECT_R);
Serial.print(", ECT [F]: ");
Serial.print(ECT_tempF);

  Serial.println(outputValue);   

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  LED_state = !LED_state;
  
  digitalWrite(LED_pin, LED_state);
  delay(1000);                     
}
