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
// - none

// Standard includes
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

/// These are from HDKLedIDentifierFactor.cpp

/// @brief Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
/// These are from the as-built measurements.
static std::vector<std::string> OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
    "X.**....*........" //  5
    ,
    "X....**...*......" //  6
    ,
    ".*...**........." //  3
    ,
    ".........*....**" //  4
    ,
    "..*.....**......" //  1
    ,
    "*......**......." //  2
    ,
    "....*.*..*......" // 10
    ,
    ".*.*.*.........." //  8
    ,
    ".........*.**..." //  9
    ,
    "X**...........*.." //  7
    ,
    "....*.*......*.." // 11
    ,
    "X*.......*.*....." // 28
    ,
    "X.*........*.*..." // 27
    ,
    "X.*.........*.*.." // 25
    ,
    "..*..*.*........" // 15
    ,
    "....*...*.*....." // 16
    ,
    "...*.*........*." // 17
    ,
    "...*.....*.*...." // 18
    ,
    "....*......*..*." // 19
    ,
    "....*..*....*..." // 20
    ,
    "X..*...*........*" // 21
    ,
    "........*..*..*." // 22
    ,
    ".......*...*...*" // 23
    ,
    "......*...*..*.." // 24
    ,
    ".......*....*..*" // 14
    ,
    "..*.....*..*...." // 26
    ,
    "*....*....*....." // 13
    ,
    "...*....*...*..." // 12
    ,
    "..*.....*...*..." // 29
    ,
    "...*......*...*." // 30
    ,
    "***...*........*" // 31
    ,
    "...****..*......" // 32
    ,
    "*.*..........***" // 33
    ,
    "**...........***" // 34
};

/// @brief Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
/// These are from the as-built measurements.
static std::vector<std::string> OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
    "X*...........**.." // 37 31 // never actually turns on in production
    ,
    "......**.*......" // 38 32
    ,
    ".............***" // 39 33
    ,
    "X..........*....." // 40 34 // never actually turns on in production
    ,
    "...*.......**..." // 33 27
    ,
    "...**.....*....." // 34 28
};

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
  auto cleanPattern = [](std::string const &inString) {
    if (inString.front() == 'X') {
      return inString.substr(1);
    }
    return inString;
  };
  std::transform(begin(OsvrHdkLedIdentifier_SENSOR0_PATTERNS),
                 end(OsvrHdkLedIdentifier_SENSOR0_PATTERNS),
                 begin(OsvrHdkLedIdentifier_SENSOR0_PATTERNS), cleanPattern);
  std::transform(begin(OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                 end(OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                 begin(OsvrHdkLedIdentifier_SENSOR1_PATTERNS), cleanPattern);

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
