/** @file
    @brief Header for array initialization

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
*/

#ifndef __ARRAY__INIT__
#define __ARRAY__INIT__

#include "Config.h" // for uint8_t without conflicts

#define LED_LINE_LENGTH 5
#define PATTERN_COUNT 16

void line_array_init(uint8_t index, uint8_t *value);
void default_array_init(void);

extern TINY uint8_t ir_led_driver_buffer[PATTERN_COUNT][LED_LINE_LENGTH * 2];
extern TINY uint8_t pattern_array[PATTERN_COUNT][LED_LINE_LENGTH];

#endif
