// AICI O SA FIE PUS CODUL FINAL:

// 1.Pozitia si directia data de quadratic encoder (DONE);
// 2.Generarea de semnal PWM (DONE);
// 3.Masurarea vitezei motorului/motoarelor (DONE);
// 4.Regulatorul pentru comanda motoarelor ;

/* Actionare motor si verificare de viteza: */
#define PI 3.14159265
int ms = 0;
float dutyRatio = 0;
long counter = 0, speed = 0;
char diskState = 0;

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
    speed = counter*10*2*PI/8; // Viteza unghiulara in rad/s
    Serial.println(String(speed) + ", " + String(dutyRatio*5));
    counter = 0;
    speed = 0;
  }
  if(ms >= 999){
    ms = 0;
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
  TCCR1B = 0b00011100;        //Prescaler de 256;
  ICR1 = 1250;                //Top value
  TCNT1 = 0;                  //Counter curent
  OCR1A = 0;                  //Valoarea la care se modifica starea (0 -> 1)
  OCR1B = 0;
}

void set_pwm(float dutyRatio){
  OCR1A = 63 * dutyRatio + 62;   // 62 tacti => 1ms ON in PWM => 0%
                                 // 125 tacti => 2ms ON in PWM => 100%
}

void mapDutyRatio(float input){ 
  dutyRatio = input/1023/5;
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
  float input_value = read_Adc(0);
  mapDutyRatio(input_value);
  set_pwm(dutyRatio);
  count();
}