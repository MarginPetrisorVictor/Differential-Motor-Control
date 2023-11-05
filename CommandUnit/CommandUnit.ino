// AICI O SA FIE PUS CODUL FINAL:

// 1.Pozitia si directia data de quadratic encoder (DONE);
// 2.Generarea de semnal PWM (DONE);
// 3.Masurarea vitezei motorului/motoarelor (DONE);
// 4.Regulatorul pentru comanda motoarelor ;

/* Actionare motor si verificare de viteza: */
#define PI 3.14159265
int ms = 0,s = 0;
volatile float dutyRatio = 0;
volatile long counter = 0, speed = 0;
volatile char diskState = 0;

/* Speed measuring + 1ms timer: */

void init_timer_1ms(){
  TCCR0A = 0b10000000;     //Timer0 CTC
  TCCR0B = 0b10000011;     //Prescaler de 64
  TCNT0 = 0;
  OCR0A = 250;             //1 ms
  TIMSK0 |= (1<<1);
  sei();
}

ISR(TIMER0_COMPA_vect){   // rot/s
  ms++;
  
  if(ms % 100 == 99){
    speed = 10 * counter/8;
    Serial.println(String(speed) + "; " + String(dutyRatio));
    counter = 0; 
  }

  if(ms >= 999){
    ms = 0;
    s++;
  }
}

void count(){
  if(PIND & (1<<7) && diskState != 1){
    counter++;
    diskState = 1;
  }
  if(!(PIND & (1<<7)))
    diskState = 0;
}

/* PWM command to the motor */

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

void init_pwm(){              //Timer 1            
  DDRB |= (1<<1);             //Setez iesire pwm PB1 (pinul 9)
  TCCR1A = 0b10000010;        //Counter normal
  TCCR1B = 0b00011010;        //Prescaler de 8;
  ICR1 = 40000;               //Top value
  TCNT1 = 0;                  //Counter curent
  OCR1A = 0;                  //Valoarea la care se modifica starea (0 -> 1)
  OCR1B = 0;
}

void set_pwm(float dutyRatio){
  if(dutyRatio != 0)
    OCR1A = 2000 * dutyRatio + 2000;
  else
    OCR1A = 2000;
}

void mapDutyRatio(float input){ 
  dutyRatio = input/10230;
}

/*Step Testing:*/

void stepSim(){
  /*  Date Identificare:
  if(s <= 10){
    set_pwm(0);
    dutyRatio = 0;
  }
  if(s > 10 && s <= 20 ){
    set_pwm(0.08);
    dutyRatio = 0.08;
  }
  if(s > 20 && s <= 30){
    set_pwm(0.04);
    dutyRatio = 0.04;
  }
  if(s > 30 && s <= 40){
    set_pwm(0.12);
    dutyRatio = 0.12;
  }
  if(s > 40 && s <= 50){
    set_pwm(0.10);
    dutyRatio = 0.10;
  }
  if(s > 50 && s <= 60){
    set_pwm(0.16);
    dutyRatio = 0.16;
  }
  if(s > 60 && s <= 70){
    set_pwm(0.08);
    dutyRatio = 0.08;
  }
  if(s > 70){
    set_pwm(0);
    dutyRatio = 0;
  }*/
  /*  Date Validare: */
  if(s <= 10){
    set_pwm(0);
    dutyRatio = 0;
  }
  if(s > 10 && s <= 20 ){
    set_pwm(0.05);
    dutyRatio = 0.05;
  }
  if(s > 20 && s <= 30){
    set_pwm(0.09);
    dutyRatio = 0.09;
  }
  if(s > 30 && s <= 40){
    set_pwm(0.06);
    dutyRatio = 0.06;
  }
  if(s > 40 && s <= 50){
    set_pwm(0.20);
    dutyRatio = 0.20;
  }
  if(s > 50 && s <= 60){
    set_pwm(0.13);
    dutyRatio = 0.13;
  }
  if(s > 60 && s <= 70){
    set_pwm(0.07);
    dutyRatio = 0.07;
  }
  if(s > 70){
    set_pwm(0);
    dutyRatio = 0;
  }
}

/* Setup */

void setup() {
  Serial.begin(115200);
  DDRD &= ~(1<<7);
  init_Adc();
  init_pwm();
  init_timer_1ms();
}

void loop() {
  /*float input_value = read_Adc(0);
  mapDutyRatio(input_value);
  set_pwm(dutyRatio);*/
  stepSim();
  count();
}