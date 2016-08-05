# IR_LED_DRV
IR LED Driver MCU

This project provides source code for IR LED board (sample/standard model powered by an STM8S003K3) that is used in the positional tracking of the OSVR HDK and other related projects.

The board flashes a pre-defined pattern on a number of LEDs. It accepts external sync signal to indicate when to switch to the next pattern.

Each LED has a "bright" state and a "dim" state, corresponding to '1' and '0' in the bit pattern.

The pattern itself can be designed using the "LED Encoding" project.

Production builds use IAR Embedded Workbench (https://www.iar.com/iar-embedded-workbench/) to compile.
It is small enough to build with the free Kickstart license, though note that you will get a sales call after registering for one.
You can also use the no-charge license for the COSMIC compiler and ST Visual Developer to build the project.

- Source code is in 'user' subdirectory
- ST-supplied library files in 'vendor' subdirectory - you will need to download and extract these frm release version 2.2.0 available at <http://www.st.com/web/en/catalog/tools/FM147/CL1794/SC1807/SS1754/PF258009> - see directory for details.
- Hardware files in 'Schematics' subdirectory
