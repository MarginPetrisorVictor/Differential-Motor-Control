// GENEREAZA UN SEMNAL PWM BAZAT PE CE CITESTE DE PE UN POTENTIOMETRU;
// De masurat frecventa cu osciloscop pentru verificare precisa!

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
  OCR1A = ICR1 * dutyRatio;
}

void setup() {
  Serial.begin(115200);
  init_Adc();
  init_pwm();
}

void loop() {
  float input_value = read_Adc(0);
  float dutyRatio = input_value/1023;
  set_pwm(dutyRatio);
}
