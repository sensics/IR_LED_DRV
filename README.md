# IR_LED_DRV
IR LED Driver MCU

This project provides source code for IR LED board that is used in the positional tracking of the OSVR HDK and other related projects.

The board flashes a pre-defined pattern on a number of LEDs. It accepts external sync signal to indicate when to switch to the next pattern.

Each LED has a "bright" state and a "dim" state, corresponding to '1' and '0' in the bit pattern.

The pattern itself can be designed using the "LED Encoding" project

It uses IAR Embedded Workbench (https://www.iar.com/iar-embedded-workbench/) to compile.

Source code is in 'user' subdirectory
IAR files in 'vendor' subdirectory

Hardware files in 'Schematics' subdirectory
