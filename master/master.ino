#include "../common/KineticRain.h"


HardwareSerial Serial1(PA10, PA9);
int angle;
int angleDelta;
char bufToSend[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Sending...");

  Serial1.write(bufToSend, sizeof(bufToSend));
  Serial1.write(0xff);
  
  delay(20);
}
