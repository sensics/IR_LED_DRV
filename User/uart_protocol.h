/** @file
    @brief Header for a byte-by-byte UART protocol

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

#ifndef __UART_PROTOCOL__
#define __UART_PROTOCOL__

#include "MCUConfig.h"

#include "stm8s.h" // for uint8_t without conflicts

#ifdef ENABLE_UART

void protocol_init();
uint8_t protocol_is_output_ready();
uint8_t protocol_get_output_byte();
void protocol_put_input_byte(uint8_t ch);

#endif

#endif
