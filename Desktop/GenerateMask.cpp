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
#include "array_init.h"
#include "PatternString.h"
#include "BeaconOrder.h"

// Library/third-party includes
// - none

// Standard includes
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using BeaconOrderContainer = std::vector<int>;

/// 1-based indices WRT the tracking software of beacons we'd like to disable.
static const auto DISABLED_TARGET0_BEACONS = {27};

static BeaconOrderContainer TargetOrder0;

int oneBasedTarget0BeaconToFirmwareBit(int beacon) {
  return TargetOrder0[beacon - 1];
}

int main() {
  TargetOrder0.assign(begin(TARGET0_BEACON_ORDER), end(TARGET0_BEACON_ORDER));
  std::bitset<NUM_LEDS> mask;
  mask.set();
  for (auto &beacon1based : DISABLED_TARGET0_BEACONS) {
    auto bit = oneBasedTarget0BeaconToFirmwareBit(beacon1based);
    mask.reset(bit);
  }

  uint64_t fullMask = mask.to_ullong();
  for (int i = 0; i < LED_LINE_LENGTH; ++i) {
    uint8_t b = static_cast<uint8_t>((0xff << (i * 8) & fullMask) >> (i * 8));
    std::cout << "0x" << std::hex << static_cast<unsigned>(b) << ", ";
  }
  std::cout << std::endl;

  return 0;
}
