# TunnelLights

Microcontroller code for a 3.007 project submission. 

## How to run
You'll need a STM32 controller to run these; they're commonly distributed as
"blue pill" boards, similar in shape to an Arduino Nano. 

You'll also need the STM32duino package for the Arduino IDE; you can find
instructions [here](https://github.com/stm32duino/Arduino_Core_STM32/wiki/Getting-Started).

## Architecture
The installation prototype consists of 25 servos and 25 WS2812 LEDs, controlled
by two STM32F103C8T6 (STM32 'blue-pill') microcontrollers. The MCUs are
themselves controlled by a host machine, running CV to move the servos and LEDs
based on the detected position of people inside the installation.

The two MCUs are arranged in a master-slave configuration: the master MCU
receives data from the host machine via USB CDC Serial, handles the half of
the servo positioning it can, then sends the rest down to the slave via another
UART.

Each MCU controls up to 13 servos using the hardware timers, while the master
MCU controls the WS2812 LEDs via the usual WS2812 driver.