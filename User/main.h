/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// All rights reserved.

#ifndef INCLUDED_main_h_GUID_8FB0D036_FAEE_4295_A01B_1F028F261122
#define INCLUDED_main_h_GUID_8FB0D036_FAEE_4295_A01B_1F028F261122

// Internal Includes
// - none

// Library/third-party includes
#include "stm8s.h"

// Standard includes
// - none

void set_flash_period(uint16_t period);
void set_blank_period(uint16_t period);
void set_interval_period(uint16_t period);
void set_interval_simulator(uint8_t simulation_period_time_ms);
uint8_t get_simulation_period();
uint16_t get_flash_period();
uint16_t get_blank_period();
uint16_t get_interval_period();

#endif // INCLUDED_main_h_GUID_8FB0D036_FAEE_4295_A01B_1F028F261122
