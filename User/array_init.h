/** @file
    @brief Header for array initialization

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015-2016 Sensics, Inc.
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

#ifndef __ARRAY__INIT__
#define __ARRAY__INIT__

#include "Config.h" // for uint8_t without conflicts

#define LED_LINE_LENGTH 5
#define DRIVER_BUFFER_LENGTH (LED_LINE_LENGTH * 2)
#define PATTERN_COUNT 16

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void line_array_init(uint8_t index, uint8_t *value);
void default_array_init(void);

extern NEAR uint8_t ir_led_driver_buffer[PATTERN_COUNT][DRIVER_BUFFER_LENGTH];
extern NEAR uint8_t pattern_array[PATTERN_COUNT][LED_LINE_LENGTH];
extern NEAR uint8_t driver_mask[DRIVER_BUFFER_LENGTH];

#ifdef __cplusplus
};     // extern "C"
#endif // __cplusplus

#endif
