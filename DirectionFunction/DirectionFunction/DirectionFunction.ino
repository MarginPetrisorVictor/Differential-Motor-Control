// Aici trebuie sa se calculeze datele primite de la Qencoder
// Trebuie implementat urmatoarele:
// 1. Structura de date pe nume directie => are un camp orientare si un camp unghi (Done)
// 2. Functie pentru maparea unghiului de la Qencoder dupa regula: 1[unit] ..... 5 [deg] (Done)
// 3. Functie pentru maparea gradelor in radiani (Done)
// 4. Functie pentru comanda calculul referintelor motoarelor (Done)
// 5. Functie pentru convertit viteza din km/h in rot/sec. (Done)
#define PI 3.14159265359
#define L 4
#define T 2
#define R 26.67

struct direction {
  char  orientation;
  int   angle;
};

struct reference{
  float leftMotorReference;
  float rightMotorReference;
};

struct direction dir;
struct reference ref;

float mapSpeedToAngularSpeed(float speed){
  return speed * 1000/R/3.6;
}

void mapDataFromQencoder(int encoderCounter){
  if(encoderCounter == 0){
    dir.orientation = 0;
    dir.angle = 0;
  }
  
  if(encoderCounter > 0){
    dir.orientation = 1;  // Merge la dreapta 
    dir.angle = encoderCounter * 5;
  }

  if(encoderCounter < 0){
    dir.orientation = -1; // Merge la stanga
    dir.angle = -encoderCounter * 5;
  }
}

float Deg2Rad(int angle){
  return angle * PI / 180;
}

void calculateRef(float speed){
  float radius = L/tan(Deg2Rad(dir.angle));
  float innerAngle = atan(L/(radius - T/2));
  float outerAngle = atan(L/(radius + T/2));
  float mappedSpeed = mapSpeedToAngularSpeed(speed);

  if(dir.orientation == 0){
    ref.leftMotorReference = mappedSpeed;
    ref.rightMotorReference = mappedSpeed;
  }

  if(dir.orientation == 1){
    //Merge la dreapta
    ref.rightMotorReference = mappedSpeed * cos(innerAngle);
    ref.leftMotorReference = mappedSpeed * cos(outerAngle);
  }

  if(dir.orientation == -1){
    //Merge la stanga
    ref.rightMotorReference = mappedSpeed * cos(outerAngle);
    ref.leftMotorReference = mappedSpeed * cos(innerAngle);
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int counter = -12;
  float speed = 10; // km/h

  mapDataFromQencoder(counter);
  calculateRef(speed);

  Serial.println(ref.leftMotorReference);
}
