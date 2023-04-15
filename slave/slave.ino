#include <Servo.h>

/**
 * Firmware for the slave controller.
 *
 * The slave controller will receive 13 values for the latter 13 
 * servos, then actuate the 13 servos according to those values.
 */


#define NUM_SERVOS 13
#define RECV_ARRAY_SIZE 13


const int SLAVE_SERVO_PINS[NUM_SERVOS] = {
  PA9,
  PA10,
  PB6,
  PB7,
  PB9,
  PB8,
  PA2,
  PA0,
  PA8,
  PA3,
  PB4,
  PA1,
  PB5
};

HardwareSerial Serial1(PB11, PB10);
char* currentBuf;
int bytes_read = 0;

bool syncByteFound = false;

HardwareTimer* tim1;
HardwareTimer* tim2;
HardwareTimer* tim3;
HardwareTimer* tim4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);
  currentBuf = (char*) malloc(sizeof(char) * RECV_ARRAY_SIZE);

  initializeTimers();
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
          setPWMValue(SLAVE_SERVO_PINS[i], currentBuf[i]);
        }
        Serial.print('\n');
        syncByteFound = false;
      
      }
    }
  }
}

void setPWMValue(int pin, int value) {
  int channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pin), PinMap_PWM));
  int valueToWrite;
  if (pin == PB9) {
    valueToWrite = map(value, 0, 255, 800, 1250);
  } else {
    valueToWrite = map(value, 0, 255, 800, 1800);
  }

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
  tim4->setCaptureCompare(4, 1250, MICROSEC_COMPARE_FORMAT);
  tim4->resume();
}
