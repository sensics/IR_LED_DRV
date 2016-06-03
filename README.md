# IR_LED_DRV
IR LED Driver MCU

This project provides source code for IR LED board (powered by an STM8S003K3) that is used in the positional tracking of the OSVR HDK and other related projects.

The board flashes a pre-defined pattern on a number of LEDs. It accepts external sync signal to indicate when to switch to the next pattern.

Each LED has a "bright" state and a "dim" state, corresponding to '1' and '0' in the bit pattern.

The pattern itself can be designed using the "LED Encoding" project

It uses IAR Embedded Workbench (https://www.iar.com/iar-embedded-workbench/) to compile. It is small enough to build with the free Kickstart license, though note that you will get a sales call after registering for one.

- Source code is in 'user' subdirectory
- ST-supplied library files in 'vendor' subdirectory
- Hardware files in 'Schematics' subdirectory
