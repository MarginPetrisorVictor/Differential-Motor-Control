long counter = 0, speed = 0;
int ms = 0;
char diskState = 0;

void timer0(){
  TCCR0A = 0b10000000;     //Timer0 CTC
  TCCR0B = 0b10000011;     //Prescaler de 64
  TCNT0 = 0;
  OCR0A = 250;             //1 ms
  TIMSK0 |= (1<<1);
  sei();
}

void count(){
  if(PIND & (1<<7) && diskState != 1){
    counter++;
    diskState = 1;
  }
  if(!(PIND & (1<<7)))
    diskState = 0;
}

ISR(TIMER0_COMPA_vect){   // rot/s
  ms++;
  if(ms > 999){
    speed = counter * 60;
    Serial.println(speed);
    counter = 0;
    speed = 0;
    ms = 0;
  }
}

void setup() {
  DDRD &= ~(1<<7);
  timer0();
  Serial.begin(9600);
}

void loop() {
  count();
}
