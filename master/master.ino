#include "../common/RecvData.h"

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


HardwareSerial Serial1(PA10, PA9);

char* recvBuf;
bool syncByteFound = false;
size_t bytes_read = 0;

RecvData* receivedData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);

  // Initialise incoming data buffers
  receivedData = (RecvData*) malloc(sizeof(RecvData));
  recvBuf = (char*) malloc(sizeof(RecvData));
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
    Serial.print("Color ");
    Serial.print(color.r, DEC);
    Serial.print(' ');
    Serial.print(color.g, DEC);
    Serial.print(' ');
    Serial.print(color.b, DEC);
    Serial.print('\n');
  }
}

/**
 * Handle the first half of the servos received.
 */
void handleMasterServos() {
  // Print positions of the first 12 servos
  for (int i = 0; i < 12; i++)
  {
    Serial.print("Position ");
    Serial.print((receivedData->positions)[i], DEC);
    Serial.print('\n');
  }
}

/**
 * Send the last half of the servos received to the slave.
 */
void sendSlaveServos() {
  Serial.println("Sending...");

  Serial1.write(0xff);

  for (int i = 12; i < 25; i++)
  {
    Serial1.write((receivedData->positions)[i]);
  }
  
  delay(20);

}
