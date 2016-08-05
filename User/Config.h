/** @file
    @brief Header

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

#ifndef INCLUDED_Config_h_GUID_9D373801_6647_4DA0_2D85_2FBB08CBAD15
#define INCLUDED_Config_h_GUID_9D373801_6647_4DA0_2D85_2FBB08CBAD15

/* Internal Includes */
/* none */

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

#ifdef __ICCSTM8__
// On IAR C compiler
#define OSVR_IR_EMBEDDED
#define OSVR_IR_IAR_STM8
#define OSVR_IR_STM8

#include "stm8s.h" // for uint8_t without conflicts

#elif defined(__CSMC__)
// On Cosmic C compiler
/// @todo detect target, not just compiler
#define OSVR_IR_EMBEDDED
#define OSVR_IR_COSMIC_STM8
#define OSVR_IR_STM8

#include "stm8s.h" // for uint8_t without conflicts

#else
// We are not on an embedded compiler
#define OSVR_IR_DESKTOP
#include <stdint.h>

// Remove embedded-only keywords
#define TINY
#define NEAR
#define EEPROM

#endif

#endif
