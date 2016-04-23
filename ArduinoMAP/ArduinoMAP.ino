const int MAP_pin = A3;
int MAP_raw = 0;
float MAP_volts = 0;
int MAP_kpa = 0;



void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode(MAP_pin, INPUT);
}

void loop() {
  MAP_raw = analogRead(MAP_pin);
  MAP_volts = 5.1f*((float)MAP_raw)/1023.0f;
  //MAP_kpa =  8+ 40.0f*5.1f*((float)MAP_raw)/1023.0f;
  MAP_kpa = 36.06f*5.1f*(((float)MAP_raw)/1023.0f) - 0.2645f;

Serial.println(MAP_kpa);
delay(100);

  
}
