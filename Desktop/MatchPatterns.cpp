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
#include "Patterns.h"

// Library/third-party includes
// - none

// Standard includes
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

int findPattern(std::vector<std::string> const &patternList,
                std::string const &pattern) {
  auto it = std::find(begin(patternList), end(patternList), pattern);
  if (it == end(patternList)) {
    return -1;
  }
  return std::distance(begin(patternList), it);
}

void dumpBeaconOrder(int targetNum, std::vector<int> const &order) {
  std::cout << "static const auto TARGET" << targetNum << "_BEACON_ORDER = {\n";

  for (auto &beacon : order) {
    std::cout << beacon << ", ";
  }
  std::cout << "};" << std::endl;
}

int main() {
  default_array_init();
  initPatterns();
  std::vector<int> targetOrder0(OsvrHdkLedIdentifier_SENSOR0_PATTERNS.size(),
                                -1);
  std::vector<int> targetOrder1(OsvrHdkLedIdentifier_SENSOR1_PATTERNS.size(),
                                -1);
  for (int led = 0; led < NUM_LEDS; ++led) {
    auto patternString = getPatternString(led, pattern_array);
    auto target0 =
        findPattern(OsvrHdkLedIdentifier_SENSOR0_PATTERNS, patternString);
    if (target0 < 0) {
      auto target1 =
          findPattern(OsvrHdkLedIdentifier_SENSOR1_PATTERNS, patternString);
      if (target1 < 0) {
        std::cout << "Could not find a match for firmware pattern " << led
                  << ": " << patternString << std::endl;
      } else {
        std::cout << "Firmware pattern " << led << " is rear target pattern "
                  << target1 << std::endl;
        targetOrder1[target1] = led;
      }
    } else {
      std::cout << "Firmware pattern " << led << " is front target pattern "
                << target0 << std::endl;
      targetOrder0[target0] = led;
    }
  }
  dumpBeaconOrder(0, targetOrder0);
  dumpBeaconOrder(1, targetOrder1);
  return 0;
}
