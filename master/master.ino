#include "../common/RecvData.h"
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

/**
 * Receive data from a PC / other serial port, and write LEDs and servos
 * accordingly.
 * 
 * Incoming data is composed of the following, in order:
 *   * 0xFF: sync byte (bytes will not be processed until this is sent)
 *   * 25 x 3 bytes representing LED colors
 *   * 25 bytes representing servo positions
 *
 * Waits for the sync byte to come in, then reads subsequent data into a generic
 * byte buffer. After all sizeof(RecvData) bytes are received, the buffer is
 * then typecast to a RecvData, a struct with the above structure (minus the
 * sync byte.)
 */


HardwareSerial Serial1(PB11, PB10);
HardwareTimer* tim1;
HardwareTimer* tim2;
HardwareTimer* tim3;
HardwareTimer* tim4;

char* recvBuf;
bool syncByteFound = false;
size_t bytes_read = 0;

RecvData* receivedData;

Adafruit_NeoPixel pixels(25, PA8, NEO_GRB + NEO_KHZ400);

#define NUM_MASTER_SERVOS 12
const int MASTER_SERVO_INDEXES[NUM_MASTER_SERVOS] = {
  0,
  12,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  23,
  24
};
const int MASTER_SERVO_PINS[NUM_MASTER_SERVOS] = {
  PB5,
  PB6,
  PB9,
  PA0,
  PB7,
  PB8,
  PA9,
  PA10,
  PB4,
  PA2,
  PA3,
  PA1,
};

#define NUM_SLAVE_SERVOS 13
const int SLAVE_SERVO_INDEXES[NUM_SLAVE_SERVOS] = {
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  13,
  22
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);

  // Initialise incoming data buffers
  receivedData = (RecvData*) malloc(sizeof(RecvData));
  recvBuf = (char*) malloc(sizeof(RecvData));

  initializeTimers();

  pixels.begin();
  pixels.clear();
  pixels.show();

  // Flash a test pattern to check LEDs
  for (int i = 0; i < 25; i++) {
    pixels.setPixelColor(i, pixels.ColorHSV(i * 2000, 255, 255));
    pixels.show();
    delay(32);
  }

  delay(3000);

  for (int i = 24; i > -1; i--) {
    pixels.setPixelColor(i, pixels.ColorHSV(0, 0, 0));
    pixels.show();
    delay(16);
  }
  pixels.show();
}

void loop() {
  // If we have bytes to read from the PC serial line...
  while (Serial.available() > 0) {
    // ... read one in.
    char receivedByte = Serial.read();


    if (!syncByteFound) {
    
      // Wait for the sync byte first, only triggering the other branch if we
      // receive it.
      syncByteFound = receivedByte == 0xff;
    
    } else {
      // Read bytes into the generic buffer first.
      recvBuf[bytes_read] = receivedByte;
      bytes_read++;
      
      // Process the bytes we've read if we have enough.
      if (bytes_read == sizeof(RecvData)) {
        bytes_read = 0;
        Serial.println("Received data: ");
        syncByteFound = false;

        // Cast our generic buffer to a RecvData.
        receivedData = (RecvData*) recvBuf;

        handleLEDs();
        handleMasterServos();
        sendSlaveServos();      
      }

    }

  }
}

/**
* Handle the LED colours received.
*/
void handleLEDs() {
  // Print colors
  for (int i = 0; i < 25; i++)
  {
    Color color = (receivedData->colors)[i];
    int hue = map(color.h, 0, 255, 0, 65535);
    int sat = map(color.s, 0, 255, 0, 65535);
    int val = map(color.v, 0, 255, 0, 65535);
    pixels.setPixelColor(i, pixels.ColorHSV(hue, sat, val));
  }
  pixels.show();
}

/**
 * Handle the first half of the servos received.
 */
void handleMasterServos() {
  // Print positions of the first 12 servos
  char* positions = receivedData->positions;

  for (int i = 0; i < NUM_MASTER_SERVOS; i++)
  {
    int index = MASTER_SERVO_INDEXES[i];
    setPWMValue(MASTER_SERVO_PINS[i], positions[index]);
  }
}

/**
 * Send the last half of the servos received to the slave.
 */
void sendSlaveServos() {
  Serial.println("Sending...");
  char* positions = receivedData->positions;

  Serial1.write(0xff);
  for (int i = 0; i < NUM_SLAVE_SERVOS; i++)
  {
    int index = SLAVE_SERVO_INDEXES[i];
    Serial1.write(positions[index]);
  }
  
  delay(20);

}



void setPWMValue(int pin, int value) {
  int channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pin), PinMap_PWM));
  int valueToWrite;
  valueToWrite = map(value, 0, 255, 800, 1800);

  switch (pin) {
  case PA8:
  case PA9:
  case PA10:
    tim1->setCaptureCompare(channel, valueToWrite, MICROSEC_COMPARE_FORMAT);
    break;
  case PA0:
  case PA1:
  case PA2:
  case PA3:
    tim2->setCaptureCompare(channel, valueToWrite, MICROSEC_COMPARE_FORMAT);
    break;
  case PB4:
  case PB5:
    tim3->setCaptureCompare(channel, valueToWrite, MICROSEC_COMPARE_FORMAT);
    break;
  case PB6:
  case PB7:
  case PB8:
  case PB9:
    tim4->setCaptureCompare(channel, valueToWrite, MICROSEC_COMPARE_FORMAT);
    break;
  }
}

void initializeTimers() {
  tim1 = new HardwareTimer(TIM1);
  tim2 = new HardwareTimer(TIM2);
  tim3 = new HardwareTimer(TIM3);
  tim4 = new HardwareTimer(TIM4);

  tim1->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA8);
  tim1->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, PA9);
  tim1->setMode(3, TIMER_OUTPUT_COMPARE_PWM1, PA10);
  tim1->setOverflow(50, HERTZ_FORMAT);
  tim1->setCaptureCompare(1, 1800, MICROSEC_COMPARE_FORMAT);
  tim1->setCaptureCompare(2, 1800, MICROSEC_COMPARE_FORMAT);
  tim1->setCaptureCompare(3, 1800, MICROSEC_COMPARE_FORMAT);
  tim1->resume();

  tim2->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PA0);
  tim2->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, PA1);
  tim2->setMode(3, TIMER_OUTPUT_COMPARE_PWM1, PA2);
  tim2->setMode(4, TIMER_OUTPUT_COMPARE_PWM1, PA3);
  tim2->setOverflow(50, HERTZ_FORMAT);
  tim2->setCaptureCompare(1, 1800, MICROSEC_COMPARE_FORMAT);
  tim2->setCaptureCompare(2, 1800, MICROSEC_COMPARE_FORMAT);
  tim2->setCaptureCompare(3, 1800, MICROSEC_COMPARE_FORMAT);
  tim2->setCaptureCompare(4, 1800, MICROSEC_COMPARE_FORMAT);
  tim2->resume();

  tim3->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PB4);
  tim3->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, PB5);
  tim3->setOverflow(50, HERTZ_FORMAT);
  tim3->setCaptureCompare(1, 1800, MICROSEC_COMPARE_FORMAT);
  tim3->setCaptureCompare(2, 1800, MICROSEC_COMPARE_FORMAT);
  tim3->resume();

  tim4->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PB6);
  tim4->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, PB7);
  tim4->setMode(3, TIMER_OUTPUT_COMPARE_PWM1, PB8);
  tim4->setMode(4, TIMER_OUTPUT_COMPARE_PWM1, PB9);
  tim4->setOverflow(50, HERTZ_FORMAT);
  tim4->setCaptureCompare(1, 1800, MICROSEC_COMPARE_FORMAT);
  tim4->setCaptureCompare(2, 1800, MICROSEC_COMPARE_FORMAT);
  tim4->setCaptureCompare(3, 1800, MICROSEC_COMPARE_FORMAT);
  tim4->setCaptureCompare(4, 1800, MICROSEC_COMPARE_FORMAT);
  tim4->resume();
}
