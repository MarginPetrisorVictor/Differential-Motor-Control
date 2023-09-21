// FUNCTIONEAZA CUM TREBUIE

// Mai trebuie adaugat salvarea pozitiei la oprire si incarcarea acesteia la initializare.

int counter = 0;
char encoderState, leftState, rightState;

void timer0(){
  TCCR0A = 0b10000000;     //Timer0 CTC
  TCCR0B = 0b10000011;     //Prescaler de 64
  TCNT0 = 0;
  OCR0A = 250;             //1 ms
  TIMSK0 |= (1<<1);
  sei();
}

ISR(TIMER0_COMPA_vect){
  char data = PIND;
  rightState = data & (1<<2);
  leftState = data & (1<<4);
}

void encoder(){
  //State 0: Left HIGH Right HIGH
  if(leftState && rightState && encoderState != 0){
    if(encoderState == 3)
      counter--;
    else
      counter++;
    encoderState = 0;
  }
  //State 1: Left LOW Right HIGH
  if(!leftState && rightState && encoderState != 1){
    encoderState = 1;
  }
  //State 2: Left LOW Right LOW
  if(!leftState && !rightState && encoderState != 2){
    encoderState = 2;
  }
  //State 3: Left HIGH Right LOW
  if(leftState && !rightState && encoderState != 3){
    encoderState = 3;
  }

  Serial.println(counter);
}

void setup(){
  DDRD &= (1<<2); //Pin PD2 pentru dreapta
  DDRD &= (1<<4); //Pin PD4 pentru stanga
  timer0();
  Serial.begin(9600);
}

void loop(){
  encoder();
}