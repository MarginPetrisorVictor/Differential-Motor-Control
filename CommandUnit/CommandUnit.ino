#define PI 3.14159265
#define BUFFERSIZE 16

union ByteToFloat{
  byte b[16];
  float value;
} u;

byte buffer[BUFFERSIZE];
int ms = 0,s = 0;
float dutyRatio = 0;
float averageSpeed = 0;
volatile long speed = 0, ref = 0;
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
//-------------SERIAL COM-----------------

void readFromMatlab(){
  int readln = Serial.readBytesUntil("\r\n",buffer,BUFFERSIZE);
  for(int index = 0; index < BUFFERSIZE; index++){
    u.b[index] = buffer[index];
  }
  dutyRatio = u.value;
  Serial.flush();
}

void writeToMatlab(float number){
  if(Serial.availableForWrite() >= 6){
    byte *b = (byte *) &number;
    Serial.write(b, 4);
    Serial.write(13);
    Serial.write(10);
    Serial.flush();
  }
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
  speed = 10*counter/8;
  averageSpeed += speed;
  counter = 0;
}

float command(float ref){
  float error = 9.755*ref - 8.8278*averageSpeed;
  float com = error/(0.06328*sq(averageSpeed)-29.11*averageSpeed+4417);
  
  if (com >= 0.3)
    com = 0.3;

  return com;
}

void setSpeed(){
  if(ms % 100 == 99){
    float mappedRef = mapSpeedReference(ref);
    Serial.println(averageSpeed);
    float com = command(ref);
    setPwm(com);
    averageSpeed = 0;
  }
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
  ref = readAdc(0);
  count();
}