/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// All rights reserved.

// Internal Includes
extern "C" {
#include "array_init.h"
};

// Library/third-party includes
// - none

// Standard includes
#include <iomanip>
#include <iostream>

int main() {
  default_array_init();

  for (int patt = 0; patt < PATTERN_COUNT; ++patt) {
    std::cout << std::setw(2) << std::setfill('0') << patt << ": ";
    for (int b = LED_LINE_LENGTH * 2 - 1; b; --b) {
      std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0')
                << int(ir_led_driver_buffer[patt][b]);
    }
    std::cout << std::endl;
  }

  return 0;
}
