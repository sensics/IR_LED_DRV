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
#include "BeaconOrder.h"
#include "PatternString.h"
#include "array_init.h"

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

/// 1-based indices WRT the tracking software of beacons we'd like to disable.
static const auto DISABLED_TARGET0_BEACONS = {33, 13, 18, 32, 34, 5};

/// 1-based indices WRT the tracking software of the beacons on the rear that never light up anyway.
static const auto DISABLED_TARGET1_BEACONS = {1, 4};

static const int BITS_PER_BYTE = 8;

/// needs a bidirectional iterator
template <typename Iterator> inline uint8_t byteFromBoolContainer(Iterator lsb) {
  uint8_t ret = 0;
  Iterator it = lsb;
  // advancing one too far is OK because of the "one past the end" iterator.
  std::advance(it, BITS_PER_BYTE);
  for (int count = BITS_PER_BYTE; count != 0; --count) {
    // can unconditionally move backwards because we moved it one too far.
    --it;
    if (*it) {
      ret = ret << 1 | static_cast<uint8_t>(0x01);
    } else {
      ret = ret << 1;
    }
  }
  return ret;
}

template <typename Container> inline uint8_t byteFromBoolContainerAtBit(Container &&c, std::size_t bit) {
  auto it = begin(std::forward<Container>(c));
  if (bit > 0) {
    std::advance(it, bit);
  }
  return byteFromBoolContainer(it);
}

template <typename Container> inline uint8_t byteFromBoolContainerAtByte(Container &&c, std::size_t b) {
  return byteFromBoolContainerAtBit(std::forward<Container>(c), b * BITS_PER_BYTE);
}

int main() {
  std::vector<bool> mask(NUM_LEDS, true);
  for (auto &beacon1based : DISABLED_TARGET0_BEACONS) {
    auto bit = oneBasedTarget0BeaconToFirmwareBit(beacon1based);
    mask[bit] = false;
  }

  for (auto &beacon1based : DISABLED_TARGET1_BEACONS) {
    auto bit = oneBasedTarget1BeaconToFirmwareBit(beacon1based);
    mask[bit] = false;
  }

  for (int i = 0; i < LED_LINE_LENGTH; ++i) {
    auto b = byteFromBoolContainerAtByte(mask, i);
    std::cout << "0x" << std::hex << static_cast<unsigned>(b) << ", ";
  }
  std::cout << std::endl;

  return 0;
}
