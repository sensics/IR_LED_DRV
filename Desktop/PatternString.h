/** @file
    @brief Header

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

#ifndef INCLUDED_PatternString_h_GUID_0101C9D0_A123_499E_F615_22480D45BCBE
#define INCLUDED_PatternString_h_GUID_0101C9D0_A123_499E_F615_22480D45BCBE

// Internal Includes
#include "array_init.h"

// Library/third-party includes
// - none

// Standard includes
#include <string>

static const auto BITS = 8;
static const auto NUM_LEDS = LED_LINE_LENGTH * BITS;

bool getElementBit(uint8_t element, uint8_t *arr) {
  auto byte = element / BITS;
  auto bit = element % BITS;
  return 0x0 != ((arr[byte]) & (0x01 << bit));
}

bool getBitFromPattern(int patternElement, int led) {
  return getElementBit(static_cast<uint8_t>(led), pattern_array[patternElement]);
}

template <typename ArrayType> std::string getPatternString(int element, ArrayType arr) {
  std::string ret;
  auto elementIdx = static_cast<uint8_t>(element);
  for (int pattElt = 0; pattElt < PATTERN_COUNT; ++pattElt) {
    ret += (getElementBit(elementIdx, arr[pattElt]) ? "*" : ".");
  }
  return ret;
}

#endif // INCLUDED_PatternString_h_GUID_0101C9D0_A123_499E_F615_22480D45BCBE
