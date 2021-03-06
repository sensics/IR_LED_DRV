/** @file
    @brief File to embed a build date/time and description in the binary

    Must be c-safe!

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
// SPDX-License-Identifier: Apache-2.0
*/

/* Internal Includes */
#include "User/MCUConfig.h"

#ifndef SYNC_DELAY_TOTAL_US
#define SYNC_DELAY_TOTAL_US 0
#endif

#define S_IMPL(X) #X
#define S(X) S_IMPL(X)
extern const char BUILD_DESC[] =
    "[Sensics IR_LED_DRV "
#ifdef PRODUCTION
    "(PRODUCTION)"
#else
    "(DEV BUILD)"
#endif
    " Built " __DATE__ " " __TIME__ ".Delay:" S(SYNC_DELAY_TOTAL_US) ",Bright:" S(FLASH_BRIGHT_PERIOD) ",Interval:" S(
        FLASH_INTERVAL_PERIOD) ",Dim:" S(FLASH_DIM_PERIOD) ",Lockout"
                                                           ":" S(FLASH_SYNC_LOCKOUT_PERIOD) "]";
