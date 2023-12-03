union ByteToFloat{
  byte b[16];
  float value;
} u;

float value = 0;
const int buffer_size = 16;
byte buf[buffer_size];

void setup(){
  Serial.begin(115200);
}

void loop(){
  readFromMatlab();
  delay(0.1);
  writeToMatlab(value+5);
}

void readFromMatlab(){
  int readln = Serial.readBytesUntil("\r\n",buf,buffer_size);
  for(int index = 0; index < buffer_size; index++){
    u.b[index] = buf[index];
  }
  value = u.value;
}

void writeToMatlab(float number){
  byte *b = (byte *) &number;
  Serial.write(b, 4);
  Serial.write(13);
  Serial.write(10);
}