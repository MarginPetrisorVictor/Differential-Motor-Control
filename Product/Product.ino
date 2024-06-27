#define _TASK_SCHEDULING_OPTIONS
#define PI 3.14159265359
#define L 4
#define T 2
#define R 0.26
#include <TaskScheduler.h>

struct UserInput{
  uint8_t speed;                  //Viteza care se citeste de la potentiometru si se mapeaza in km/h
  int qCounter;                   //Nr. de impulsuri de la Quadrature Encoder mapat dupa regula 1[imp] .... 5[grd]
};

struct QuadratureEncoder{         
  char encoderState;              //Starea encoder-ului 
  bool leftState;                 //Starea senzorului din stanga
  bool rightState;                //Starea senzorului din dreapta
  int qEncoderCounter;            //Nr. de impulsuri citite (pentru mers la drepta ++ pentru mers la stanga --)
};

struct Data{
  uint8_t angularSpeed;           //Viteza unghiulara = f(speed), speed este viteza data de user
  uint8_t angle;                  //Unghiul in grade pregatit de calcul
  char orientation;               //Determina orientarea in care se merge: 1 - merge spre dreapta; 0 - daca merge drept; -1 - daca merge la stanga
};

struct References{
  uint8_t leftMotorRef;           //Referinta motorului stang
  uint8_t rightMotorRef;          //Referinta motorului drept
};

struct MeasuredSpeed{           
  uint8_t leftMotorActualSpeed;   //Viteza actuala a motorului stang;
  uint8_t rightMotorActualSpeed;  //Viteza actuala a motorului drept;
};

struct Command{
  float leftMotorCommand;         //Comanda care trebuie data spre driver-ul motorului stang
  float rightMotorCommand;        //Comanda care trebuie data spre driver-ul motorului drept
};

struct SpeedEncoder{
  uint8_t leftMotorCounter;       //Numarul de impulsuri citit pentru motorul din stanga (max 250[rot/s] => 2000[imp/s])
  char leftDiskState;             //Starea encoder-ului asignat motorului stang
  uint8_t rightMotorCounter;      //Numarul de impulsuri citit pentru motorul din dreapta
  char rightDiskState;            //Starea encoder-ului asignat motorului drept
};
                    
struct Data data;
struct References refs;
struct SpeedEncoder speedEncoder;
struct QuadratureEncoder quadratureEncoder;

void readFastQEncoder();
void readInputData();                 
void setReferences();
void setCommand();
void readWithHighSpeed();
uint8_t mapInRegions(uint16_t value);
int readQencoder(struct QuadratureEncoder* qep);
struct UserInput readAdcAndQEncoder(struct QuadratureEncoder* qep);
struct Data mapDataFromUser(struct UserInput input);
float Deg2Rad(int angle);
struct References calculateRefs(struct Data* dp);
struct MeasuredSpeed mapImpulsesToSpeed(struct SpeedEncoder* sep);
float threshold(float ref);
float correctCommand(float testedCommand);
float regulator(uint8_t ref, uint8_t speed);
struct Command calculateCommands(struct MeasuredSpeed measuredSpeed, struct References* rp);
void setToDrivers(struct Command command);
void interogateLeftEncoder(struct SpeedEncoder* sep);
void interogateRightEncoder(struct SpeedEncoder* sep);
void countImpulses(struct SpeedEncoder* sep);
void Qencoder(struct QuadratureEncoder* qep);
uint16_t readAdc(char channel);
void setLeftMotorCommand(float command);
void setRightMotorCommand(float command);
void _initPins();
void _initMotors();
void _initAdc();
void _initQencoder();
void _initTasks();

Scheduler schedule;

Task readInputTask(200, TASK_FOREVER, &readInputData);
Task setRefsTask(100, TASK_FOREVER, &setReferences);
Task setCommandTask(100, TASK_FOREVER, &setCommand);
Task readSpeedTask(0.5, TASK_FOREVER, &readWithHighSpeed);
Task readQuadratureEncoderTask(1, TASK_FOREVER, &readFastQEncoder);

void setup() {                        
  Serial.begin(115200);
  _initPins();
  _initMotors();
  _initAdc();
  _initQencoder();
  _initTasks();
}

void loop() {
  schedule.execute();
}

//-----------------------------------TASKS----------------------------------------
//Prioritate: 4 (Scazuta)
//Se citeste quadrature encoder-ul
//Se executa o data la 1ms
void readFastQEncoder(){
  Qencoder(&quadratureEncoder);
}

//Prioritate: 3 (Scazuta)
//Se citesc intrarile de la user: potentiometru, Qencoder
//Se mapeaza datele de la user
//Se repeta o data la 200 ms;
void readInputData(){
  struct UserInput input = readAdcAndQEncoder(&quadratureEncoder); 
  data = mapDataFromUser(input);
}

//Prioritate: 2 (Mediu)
//Calculeaza referintele motoarelor
//Se executa o data la 150ms 
void setReferences(){
  refs = calculateRefs(&data);
  //Serial.println(String(refs.leftMotorRef)+" "+String(refs.rightMotorRef));
}

//Prioritate: 1 (Mare)
//Se citeste referinta
//Se citeste viteza la momentul calcului
//Se calculeaza comanda
//Se seteaza comanda pe motor (PWM la driver)
//Trebuie executata o data la 100ms
void setCommand(){
  struct MeasuredSpeed measuredSpeed = mapImpulsesToSpeed(&speedEncoder);
  struct Command command = calculateCommands(measuredSpeed, &refs); 
  setToDrivers(command);
  Serial.println(String(measuredSpeed.leftMotorActualSpeed)+" "+String(measuredSpeed.rightMotorActualSpeed)+" "+String(data.angle));
}

//Prioritate: 0 (Cea mai mare)
//Citeste viteza motoarelor
//Se executa cu viteza de 0.5ms => Top measured speed = 250 rot/s
void readWithHighSpeed(){
  countImpulses(&speedEncoder);
}

//-------------------------------UTILITIES----------------------------------------
uint8_t mapInRegions(uint16_t value){
  uint8_t speed = 0;
  if(223 <= value && value < 423)
    speed = 132;
  if(423 <= value && value < 623)
    speed = 152;
  if(623 <= value && value < 823)
    speed = 172;
  if(823 <= value && value <= 1023)
    speed = 192;

  return speed;
}

int readQencoder(struct QuadratureEncoder* qep){
  int count = qep->qEncoderCounter;
  return count;
}

struct UserInput readAdcAndQEncoder(struct QuadratureEncoder* qep){
  struct UserInput newInputs;

  newInputs.speed = mapInRegions(readAdc(0));
  newInputs.qCounter = readQencoder(qep);

  return newInputs;
}

struct Data mapDataFromUser(struct UserInput input){
  struct Data mappedData;
  
  mappedData.angularSpeed = input.speed/R/3.6;

  switch(input.qCounter){
    case 0:
      mappedData.orientation = 0;
      mappedData.angle = 0;
      break;
    default:
      if(input.qCounter > 0)
        mappedData.orientation = 1;
      else
        mappedData.orientation = -1;
      mappedData.angle = abs(input.qCounter);
      break;
  }

  return mappedData;
}

float Deg2Rad(int angle){
  return angle*PI/180;
}

struct References calculateRefs(struct Data* dp){
  struct References calculatedRefs;
  float curveRadius = L/tan(Deg2Rad(dp->angle));
  float innerAngle = atan(L/(curveRadius - T/2));
  float outerAngle = atan(L/(curveRadius + T/2));
  switch(dp->orientation){
    case 0:
      calculatedRefs.leftMotorRef = dp->angularSpeed;
      calculatedRefs.rightMotorRef = dp->angularSpeed;
      break;
    case 1:
      calculatedRefs.leftMotorRef = (float)dp->angularSpeed * cos(outerAngle);
      calculatedRefs.rightMotorRef = (float)dp->angularSpeed * cos(innerAngle);
      break;
    case -1:
      calculatedRefs.leftMotorRef = (float)dp->angularSpeed * cos(innerAngle);
      calculatedRefs.rightMotorRef = (float)dp->angularSpeed * cos(outerAngle);
      break;
  }
  
  return calculatedRefs; 
}

struct MeasuredSpeed mapImpulsesToSpeed(struct SpeedEncoder* sep){
  struct MeasuredSpeed newSpeeds;
  newSpeeds.leftMotorActualSpeed = sep->leftMotorCounter * 1.25;
  newSpeeds.rightMotorActualSpeed = sep->rightMotorCounter * 1.25;

  sep->leftMotorCounter = 0;
  sep->rightMotorCounter = 0;

  return newSpeeds;
}

float thresholdLeft(float ref){
  return ref/(0.06328*sq(ref)-29.11*ref+4417);
}

float thresholdRight(float ref){
  return ref/(0.06328*sq(ref)-29.11*ref+5200);
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

float regulatorLeft(uint8_t ref, uint8_t speed){
  float error = 2.5439* /*4.904*/(float)ref - 1.5442* /*3.903882032022076*/(float)speed;
  float com = error/(0.06328*sq((float)speed)-29.11*(float)speed+4417);
  float thresholdCommand = thresholdLeft((float)ref);
  
  if(com < thresholdCommand)
    com = thresholdCommand;
  
  com = correctCommand(com);

  if(com >= 0.2)
    com = 0.2;
  
  if(com < 0.06)
    com = 0;
    
  return com;
}

float regulatorRight(uint8_t ref, uint8_t speed){
  float error = 2.5439* /*4.904*/(float)ref - 1.5442* /*3.903882032022076*/(float)speed;
  float com = error/(0.06328*sq((float)speed)-29.11*(float)speed+5200);
  float thresholdCommand = thresholdRight((float)ref);
  
  if(com < thresholdCommand)
    com = thresholdCommand;
  
  com = correctCommand(com);

  if(com >= 0.2)
    com = 0.2;
  
  if(com < 0.06)
    com = 0;
    
  return com;
}

struct Command calculateCommands(struct MeasuredSpeed measuredSpeed, struct References* rp){
  struct Command com;
  
  if(data.angle == 0){
    com.leftMotorCommand = regulatorLeft(rp->leftMotorRef, measuredSpeed.leftMotorActualSpeed);
    com.rightMotorCommand = regulatorRight(rp->leftMotorRef, measuredSpeed.rightMotorActualSpeed);
  }else{
  com.leftMotorCommand = regulatorLeft(rp->leftMotorRef, measuredSpeed.leftMotorActualSpeed);
  com.rightMotorCommand = regulatorRight(rp->rightMotorRef, measuredSpeed.rightMotorActualSpeed);
  }
  return com;
}

void setToDrivers(struct Command command){
  setLeftMotorCommand(command.leftMotorCommand);
  setRightMotorCommand(command.rightMotorCommand);
}

void interogateLeftEncoder(struct SpeedEncoder* sep){
  if(PIND & (1<<7) && sep->leftDiskState != 1){
    sep->leftMotorCounter++;
    sep->leftDiskState = 1;
  }

  if(!(PIND & (1<<7)))
    sep->leftDiskState = 0;
}

void interogateRightEncoder(struct SpeedEncoder* sep){
  if(PIND & (1<<4) && sep->rightDiskState != 1){
    sep->rightMotorCounter++;
    sep->rightDiskState = 1;
  }

  if(!(PIND & (1<<4)))
    sep->rightDiskState = 0;
}

void countImpulses(struct SpeedEncoder* sep){
  interogateLeftEncoder(sep);
  interogateRightEncoder(sep);
}

void Qencoder(struct QuadratureEncoder* qep){
  char data = PIND;
  qep->rightState = data & (1<<2);
  qep->leftState = data & (1<<3);
  
  //State 0: Left HIGH Right HIGH
  if(qep->leftState && qep->rightState && qep->encoderState != 0){
    if(qep->encoderState == 3)
      qep->qEncoderCounter++;
    else
      qep->qEncoderCounter--;
    qep->encoderState = 0;
  }
  //State 1: Left LOW Right HIGH
  if(!qep->leftState && qep->rightState && qep->encoderState != 1){
    qep->encoderState = 1;
  }
  //State 2: Left LOW Right LOW
  if(!qep->leftState && !qep->rightState && qep->encoderState != 2){
    qep->encoderState = 2;
  }
  //State 3: Left HIGH Right LOW
  if(qep->leftState && !qep->rightState && qep->encoderState != 3){
    qep->encoderState = 3;
  }
}

//--------------------------------CONFIG------------------------------------------
void _initPins(){
  DDRB |= (1<<1);                  //PWM pin 9 pentru motorul stang
  DDRB |= (1<<2);                  //PWM pin 10 pentru motorul drept
  DDRD &= ~(1<<7);                 //Speed Encoder pentru motorul stang
  DDRD &= ~(1<<4);                 //Speed Encoder pentru motorul drept
}

void _initQencoder(){
  DDRD &= ~(1<<2);                 //Qencoder pin 2 pentru directia de dreapta
  DDRD &= ~(1<<3);                 //Qencoder pin 3 pentru directia de stanga
}

void _initAdc(){
  ADMUX = 0b01000000;         
  ADCSRA |= (1<<7);
}

uint16_t readAdc(char channel){
  ADMUX &= 0b01000000;        
  ADMUX |= channel;           
  ADCSRA |= (1<<6);           
  while(ADCSRA & (1<<6))      
  return (ADCL | (ADCH << 8));    
}

void _initMotors(){                                       
  TCCR1A = 0b10100010;        
  TCCR1B = 0b00011010;        
  ICR1 = 40000;               
  TCNT1 = 0;                  
  OCR1A = 0;                  
  OCR1B = 0;
}

void setLeftMotorCommand(float command){
  if(command != 0)
    OCR1A = 2000 * command + 2000;
  else
    OCR1A = 2000;
}

void setRightMotorCommand(float command){
  if(command != 0)
    OCR1B = 2000 * command + 2000;
  else
    OCR1B = 2000;
}

void _initTasks(){
  readSpeedTask.setSchedulingOption(TASK_SCHEDULE);
  setCommandTask.setSchedulingOption(TASK_SCHEDULE);
  setRefsTask.setSchedulingOption(TASK_SCHEDULE);
  readInputTask.setSchedulingOption(TASK_SCHEDULE);
  readQuadratureEncoderTask.setSchedulingOption(TASK_SCHEDULE);

  schedule.init();
  schedule.addTask(readSpeedTask);
  schedule.addTask(setCommandTask);
  schedule.addTask(setRefsTask);
  schedule.addTask(readInputTask);
  schedule.addTask(readQuadratureEncoderTask);

  readSpeedTask.enable();
  setCommandTask.enable();
  setRefsTask.enable();
  readInputTask.enable();
  readQuadratureEncoderTask.enable();
}