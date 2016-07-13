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

// Library/third-party includes
#include <json/value.h>
#include <json/writer.h>

// Standard includes
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

static const auto TARGET0_BEACON_ORDER = {
    5,  6,  3,  4,  1,  2,  10, 8,  9,  7,  11, 28, 27, 25, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 14, 26, 13, 12, 29, 30, 31, 32, 33, 34};
static const auto TARGET1_BEACON_ORDER = {37, 38, 39, 40, 33, 34};

int main() {
  default_array_init();
#if 0
    std::vector<std::string> patterns(NUM_LEDS);
    for (int patternElement = 0; patternElement < PATTERN_COUNT; ++patternElement) {
        for (int led = 0; led < NUM_LEDS; ++led) {
            patterns[led] += (getBitFromPattern(patternElement, led) ? "*" : ".");
        }
    }


    for (int led = 0; led < NUM_LEDS; ++led) {
        std::cout << "LED " << std::setw(2) << led << ": " << patterns[led]  << std::endl;
    }
#endif
  for (int led = 0; led < NUM_LEDS; ++led) {
    std::cout << "pattern_array LED " << std::setw(2) << led << ": "
              << getPatternString(led, pattern_array) << std::endl;
  }
  std::cout << "\n\n";

  {
    Json::Value root;
    auto addBeacon = [&](int beaconId) {
      Json::Value beacon;
      beacon[std::to_string(beaconId)] =
          getPatternString(beaconId - 1, pattern_array);
      root.append(beacon);
    };
    for (auto beaconId : TARGET0_BEACON_ORDER) {
      // root[std::to_string(beaconId)] = getPatternString(beaconId - 1,
      // pattern_array);
      addBeacon(beaconId);
    }
    for (auto beaconId : TARGET1_BEACON_ORDER) {
      // root[std::to_string(beaconId)] = getPatternString(beaconId - 1,
      // pattern_array);

      addBeacon(beaconId);
    }
    std::ofstream of("beacons.json");
    if (of) {
      of << root.toStyledString();
      std::cout << "Wrote beacons.json file." << std::endl;
    }
  }

  for (int elt = 0; elt < 2 * NUM_LEDS; ++elt) {
    std::cout << "ir_led_driver_buffer " << std::setw(2) << elt << ": "
              << getPatternString(elt, ir_led_driver_buffer) << std::endl;
  }

  return 0;
}
