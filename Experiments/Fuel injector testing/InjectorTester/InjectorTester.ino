/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 12;
int startDelay = 3;
int PUMP_pin =11;
int numberOfTicks = 500;
int tickDuration = 20;

// the setup routine runs once when you press reset:
void setup() {       
Serial.begin(9600);  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
digitalWrite(led,0);
  
  pinMode(PUMP_pin, OUTPUT); 
digitalWrite(PUMP_pin, 0);  
}

// the loop routine runs over and over again forever:
void loop() {
  
  
    digitalWrite(PUMP_pin, 1);
    Serial.println("PUMP OM");

  for(int i=0; i<startDelay; i++) {
    Serial.print("Starting in ");
      Serial.println( startDelay-i );
      delay(1000);
  }
  
  for(int i=0; i<numberOfTicks; i++) {
    Serial.print("Tick: ");
    Serial.println(i+1);
//    Serial.println("ON");
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(tickDuration);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//    Serial.println("OFF");
    delay(10);               // wait for a second
  }
  
  digitalWrite(PUMP_pin, 0);
      Serial.println("PUMP OFF");

  
  Serial.println("DONE!");
  while(1){}
}
