#include <Servo.h>

/**
 * Firmware for the slave controller.
 *
 * The slave controller will receive 13 values for the latter 13 
 * servos, then actuate the 13 servos according to those values.
 */


#define NUM_SERVOS 13
#define RECV_ARRAY_SIZE 13

int servo_pins[NUM_SERVOS] = {PA2, PA3};
HardwareSerial Serial1(PA10, PA9);
char* currentBuf;
int bytes_read = 0;

bool syncByteFound = false;

Servo servos[NUM_SERVOS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);
  currentBuf = (char*) malloc(sizeof(char) * RECV_ARRAY_SIZE);

  delay(3000);

  // for (int i = 0; i < NUM_SERVOS; i++)
  // {
  //   Serial.print("Initialising servo #");
  //   Serial.println(i);
  //   servos[i] = Servo();
  //   servos[i].attach(servo_pins[i]);
  // }
  Serial.println("Starting...");
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available() > 0) {
    char receivedByte = Serial1.read();

    if (!syncByteFound) {
      syncByteFound = receivedByte == 0xff;
    } else {
      currentBuf[bytes_read] = receivedByte;
      bytes_read++;
      
      if (bytes_read == RECV_ARRAY_SIZE) {
      
        bytes_read = 0;
        Serial.print("Received data: ");
        for (int i = 0; i < RECV_ARRAY_SIZE; i++)
        {
          Serial.print(currentBuf[i], DEC);
          Serial.print(' ');
        }
        Serial.print('\n');
        syncByteFound = false;
      
      }

    }

  }
}
