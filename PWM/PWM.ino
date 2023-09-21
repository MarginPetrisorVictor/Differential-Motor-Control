int s = 0;
int c = 1;
void init_timer(){
  TCCR0A = 0b00000000;  //Counter normal
  TCCR0B = 0b00000101;  //Prescaler de 1024
  TCNT0 = 0;
}

void setup() {
  Serial.begin(115200);
  init_timer();
}

void loop() {
   if(TCNT0 >= 78){
    TCNT0 = 0;
    c++;
  }
  
  if(TCNT0 >= 20){
    s = 1;
  }
  else 
    s = 0;
  if(c <= 50)
    Serial.println(s);
}
