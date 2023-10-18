// Serial communication:
// Transmit date o data pe ms

int ms = 0;
void init_Adc(){
  ADMUX = 0b01000000;         //Setez tensiunea de referinta (AVcc)
  ADCSRA |= (1<<7);           //Enable la convertorul analogic digital
}

int read_Adc(char channel){
  ADMUX &= 0b01000000;        //Resetez canalul de convertire
  ADMUX |= channel;           //Setez canalul de pe care citesc
  ADCSRA |= (1<<6);           //Pornesc conversia
  while(ADCSRA & (1<<6))      //Astept finalul conversiei
  return (ADCL | (ADCH << 8));
}

void init_timer0(){
  TCCR0A = 0b10000000;     //Timer0 CTC
  TCCR0B = 0b10000011;     //Prescaler de 64
  TCNT0 = 0;
  OCR0A = 250;             //1 ms
  TIMSK0 |= (1<<1);
  sei();
}

ISR(TIMER0_COMPA_vect){
  ms++;
  float input_value = read_Adc(0);
  float dutyRatio = input_value/1023;
  Serial.println(String(dutyRatio)); //+ ", " + ms);  
}

void setup() {
  Serial.begin(9600);
  init_Adc();
  init_timer0();
}

void loop() {
}
