
uint16_t VRS0_dtms[]={ 14, 11, 12, 10, 11, 11, 12, 10, 14, 11, 10, 7, 7, 6, 8, 8, 8, 8, 8, 10, 10, 9, 9, 6, 7, 6, 7, 7, 8, 7, 9, 9, 9, 9, 8, 8, 6, 5, 7, 6, 8, 8, 8, 8, 10, 8, 9, 6, 7, 5, 6, 7, 9, 7, 8, 8, 9, 10, 8, 6, 6, 6, 6, 6, 9, 7, 9, 8, 9, 10, 8, 6, 6, 6, 6, 8, 8, 7, 8, 9, 10, 9, 8, 6, 7, 5, 8, 6, 8, 7, 8, 8, 11, 8, 9, 6, 6, 5, 8, 6, 8, 7, 9, 9, 10, 8, 9, 6, 7, 5, 7, 6, 8, 7, 10, 8, 9, 9, 8, 7, 7, 6, 6, 7, 8, 7, 10, 8, 10, 8, 10, 6, 7, 5, 7, 6, 8, 8, 9, 8, 9, 9, 9, 7, 6, 5, 7, 7, 9, 7, 8, 8, 10, 10, 9, 6, 6, 6, 6, 8, 8, 7, 8, 8, 11, 9, 9, 6, 6, 6, 8, 6, 9, 7, 8, 9, 10, 9, 9, 6, 7, 6, 7, 6, 9, 6, 9, 9};
uint16_t VRS1_dtms[]={72, 12, 12, 86, 97, 83, 73, 12, 13, 87, 97, 83, 0, 76, 12, 13, 90, 96, 84, 75, 12, 13, 88, 99, 84, 75, 13, 13, 89, 98, 63, 20};

volatile uint16_t VRS0_i = 0;
volatile uint16_t VRS1_i = 0;

ISR(TIMER0_COMPA_vect) {
  OCR0A = VRS0_dtms[VRS0_i];
  VRS0_i = (VRS0_i + 1)%(12*6);
}

ISR(TIMER2_COMPA_vect) {
  OCR2A = VRS1_dtms[VRS1_i];
  VRS1_i = (VRS1_i + 1)%6;
}

// the setup function runs once when you press reset or power the board
void setup() {
  cli();
  DDRD |= (1<<6); // PD6 output
  PORTD &= ~(1<<6); // PD6 = 0
  DDRD |= (1<<3); // PD3 output
  PORTD &= ~(1<<3); // PD3 = 0

  DDRB |= (1<<3); // PB3 output
  PORTB &= ~(1<<3); // PB3 = 0


  TCCR2A = (1<<COM2A0) | (1<<WGM21); // OCR2A on compare match, CTC
  TCCR2B = (1<<CS22) | (1<<CS21);// | (1<<CS20); // /1024
  TIMSK2 = (1<<OCIE2A); // interrupt on compare

  TCCR0A = (1<<COM0A0) |  (1<<WGM01); // toggle OCR0A on compare match, CTC
  TIMSK0 = (1<<OCIE0A) | (1<<OCIE0B); // interrupt on compare match A
  
  TCCR0B = (1<<CS02) ;//| (1<<CS00); // /1024 // and start
  sei();
}


uint8_t VRS0_state = 0;

// the loop function runs over and over again forever
void loop() {
}
