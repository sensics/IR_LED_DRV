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

#ifndef INCLUDED_Positions_h_GUID_76FF5E5E_984A_429D_CD2D_7696286A97F8
#define INCLUDED_Positions_h_GUID_76FF5E5E_984A_429D_CD2D_7696286A97F8

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <array>
#include <vector>

using Point3 = std::array<double, 3>;
using Point3Vector = std::vector<Point3>;

// clang-format off
// Default 3D locations for the beacons on an OSVR HDK face plate, in
// millimeters
// Appears to correspond to manufacturing specs starting at LED7
const Point3Vector OsvrHdkLedLocations_SENSOR0 = {
    Point3{ -84.93, 2.65, 23.59 }, // LED7 - updated 20151211
    Point3{ -83.27, -14.33, 13.89 }, // LED8 - updated 20151211
    Point3{ -47, 51, 24.09 },
    Point3{ 47, 51, 24.09 },
    Point3{ 84.93, 2.65, 23.59 },// LED11 - updated 20151211
    Point3{ 83.27, -14.33, 13.89 }, // LED12 - updated 20151211
    Point3{ 84.1, 19.76, 13.89 }, // LED13 - updated 20151211
    Point3{ 21, 51, 24.09 },  // Original spec was 13.09, new position works better
    Point3{ -21, 51, 24.09 }, // Original spec was 13.09, new position works better
    Point3{ -84.1, 19.76, 13.89 }, // LED16
    Point3{ -60.41, 47.55, 44.6 },
    Point3{ -80.42, 20.48, 42.9 },
    Point3{ -82.01, 2.74, 42.4 },
    Point3{ -80.42, -14.99, 42.9 },
    Point3{ -60.41, -10.25, 48.1 },
    Point3{ -60.41, 15.75, 48.1 },
    Point3{ -30.41, 32.75, 50.5 },
    Point3{ -31.41, 47.34, 47 },
    Point3{ -0.41, -15.25, 51.3 },
    Point3{ -30.41, -27.25, 50.5 },
    Point3{ -60.44, -41.65, 45.1 },
    Point3{ -22.41, -41.65, 47.8 },
    Point3{ 21.59, -41.65, 47.8 },
    Point3{ 59.59, -41.65, 45.1 },
    Point3{ 79.63, -14.98, 42.9 },
    Point3{ 29.59, -27.25, 50.5 },
    Point3{ 81.19, 2.74, 42.4 },
    Point3{ 79.61, 20.48, 42.9 },
    Point3{ 59.59, 47.55, 44.6 },
    Point3{ 30.59, 47.55, 47 },
    Point3{ 29.59, 32.75, 50.5 },
    Point3{ -0.41, 20.75, 51.3 },
    Point3{ 59.59, 15.75, 48.1 },
    Point3{ 59.59, -10.25, 48.1 }
};

// Default 3D locations for the beacons on an OSVR HDK back plate, in
// millimeters
const Point3Vector OsvrHdkLedLocations_SENSOR1 = {
    Point3{ 1, 23.8, 0 }, // never actually turns on in production
    Point3{ 11, 5.8, 0 },
    Point3{ 9, -23.8, 0 },
    Point3{ 0, -8.8, 0 }, // never actually turns on in production
    Point3{ -9, -23.8, 0 },
    Point3{ -12, 5.8, 0 }
};

// clang-format on

#endif // INCLUDED_Positions_h_GUID_76FF5E5E_984A_429D_CD2D_7696286A97F8
