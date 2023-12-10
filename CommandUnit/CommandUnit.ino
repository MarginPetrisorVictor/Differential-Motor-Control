#define PI 3.14159265

int ms = 0,s = 0;
float dutyRatio = 0, averageSpeed = 0;
volatile float speed = 0, ref = 0, commandThreshold = 0;
volatile long counter = 0;
volatile char diskState = 0;

//---------------1msTIMER-----------------

void init_timer_1ms(){
  TCCR0A = 0b10000000;     
  TCCR0B = 0b10000011;    
  TCNT0 = 0;
  OCR0A = 250;             
  TIMSK0 |= (1<<1);
  sei();
}

ISR(TIMER0_COMPA_vect){
  ms++;

  readSpeed();
  setSpeed();

  if(ms >= 999){
    ms = 0;
    s++;
  }
}
//----------------------------------------
//-------------MANUAL INPUT---------------

void initAdc(){
  ADMUX = 0b01000000;         
  ADCSRA |= (1<<7);           
}

int readAdc(char channel){
  ADMUX &= 0b01000000;        
  ADMUX |= channel;           
  ADCSRA |= (1<<6);           
  while(ADCSRA & (1<<6))      
  return (ADCL | (ADCH << 8));
}

//----------------------------------------
//-----------------PWM--------------------

void initPwm(){                          
  DDRB |= (1<<1);             
  TCCR1A = 0b10000010;        
  TCCR1B = 0b00011010;        
  ICR1 = 40000;               
  TCNT1 = 0;                  
  OCR1A = 0;                  
  OCR1B = 0;
}

void setPwm(float ratio){
  if(ratio != 0)
    OCR1A = 2000 * ratio + 2000;
  else
    OCR1A = 2000;
}

//----------------------------------------
//---------------UTILITIES----------------

float mapSpeedReference(float input){ 
  return input/5;
}

void count(){
  if(PIND & (1<<7) && diskState != 1){
    counter++;
    diskState = 1;
  }
  if(!(PIND & (1<<7)))
    diskState = 0;
}

//----------------------------------------
//--------------CONTROLLER----------------

void readSpeed(){
  speed = 1.25*counter;
  averageSpeed += speed;
  counter = 0;
}

void setThreshold(float ref){
  commandThreshold = ref/(0.06328*sq(ref)-29.11*ref+4417);
}

float correctCommand(float testedCommand){
  int integerCommand = (int) 2000 * testedCommand;
  float floatCommand = 2000*testedCommand;
  float returnedCommand = testedCommand;

  if(floatCommand - integerCommand > 0.5)
    returnedCommand = (float) (integerCommand + 1)/2000;

  if(floatCommand - integerCommand < 0.5)
    returnedCommand = (float) integerCommand/2000;

  return returnedCommand;
}

float command(float ref){
  float error = 4.904*ref - 3.903882032022076*averageSpeed;
  float com = error/(0.06328*sq(averageSpeed)-29.11*averageSpeed+4417);

  if(com < commandThreshold || error < ref + 50)
    com = commandThreshold;

  com = correctCommand(com);

  if (com >= 0.2)
    com = 0.2;

  if(com < 0)
    com = 0;

  return com;
}

void setSpeed(){
  if(ms % 100 == 99){
    if(s < 10)
      ref = 0;
    if(s > 10 && s < 20)
      ref = 120;
    if(s > 20)
      ref = 0;
    setThreshold(ref);
    float com = command(ref);
    Serial.println(String(averageSpeed) + "; " + String(com));
    setPwm(com);
    averageSpeed = 0;
  }
}

void setReference(int reference){
  ref = reference;
}

//----------------------------------------

void setup() {
  Serial.begin(115200);
  DDRD &= ~(1<<7);
  initAdc();
  initPwm();
  init_timer_1ms();
  setPwm(0);
}

void loop() {
  count();
}