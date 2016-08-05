/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache 2.0

// Internal Includes
#include "array_init.h"

// Library/third-party includes
// - none

// Standard includes
#include <iomanip>
#include <iostream>

int main() {
  default_array_init();

  for (int patt = 0; patt < PATTERN_COUNT; ++patt) {
    std::cout << std::setw(2) << std::setfill('0') << patt << ": ";
    for (int b = 0; b < DRIVER_BUFFER_LENGTH; ++b) {
      std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0') << int(ir_led_driver_buffer[patt][b]);
    }
    std::cout << std::endl;
  }

  return 0;
}
