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

#ifndef INCLUDED_Patterns_h_GUID_7F4808CB_757C_4D82_2F6E_C10D805D686F
#define INCLUDED_Patterns_h_GUID_7F4808CB_757C_4D82_2F6E_C10D805D686F

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>
#include <vector>
#include <string>

/// These are from HDKLedIDentifierFactor.cpp

/// @brief Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
/// These are from the as-built measurements.
std::vector<std::string> OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
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
std::vector<std::string> OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
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

const auto NUM_LEDS_SENSOR0 = OsvrHdkLedIdentifier_SENSOR0_PATTERNS.size();

const auto PATTERN_LENGTH = OsvrHdkLedIdentifier_SENSOR0_PATTERNS.back()
                                .length(); // the back LED is enabled.

void initPatterns() {
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
}

#endif // INCLUDED_Patterns_h_GUID_7F4808CB_757C_4D82_2F6E_C10D805D686F
