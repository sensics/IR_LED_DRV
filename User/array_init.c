/** @file
    @brief Implementation

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

/* Internal Includes */
#include "array_init.h"

/* Library/third-party includes */
#ifdef OSVR_IR_STM8
#include "stm8s.h"
#endif

/* Standard includes */
/* - none - */

// clang-format off
EEPROM uint8_t default_mask[LED_LINE_LENGTH] = {0xff, 0xff, 0xff, 0xff, 0xff};

#if 1
EEPROM uint8_t default_pattern_array[PATTERN_COUNT][LED_LINE_LENGTH] =
{
    {29,0,32,136,16},
    {9,0,192,26,4},
    {69,72,1,64,4},
    {162,0,12,2,3},
    {2,48,18,1,10},
    {18,0,5,18,8},
    {3,66,16,17,32},
    {0,37,1,128,36},
    {96,136,34,192,0},
    {2,0,8,101,40},
    {144,2,98,0,130},
    {0,153,136,4,1},
    {96,36,64,4,17},
    {12,2,144,8,80},
    {140,144,4,32,64},
    {13,69,0,32,64}
};
#else
/// Pattern for testing sync.
EEPROM uint8_t default_pattern_array[PATTERN_COUNT][LED_LINE_LENGTH] =
{
    {0x03,0,0x30,0,0},
    {0x03,0,0x30,0,0},
    {0,0x03,0,0,0x30},
    {0,0x03,0,0,0x30},
    {0x30,0,0x03,0,0},
    {0x30,0,0x03,0,0},
    {0,0x30,0,0x03,0},
    {0,0x30,0,0x03,0},
    {0,0,0x30,0,0x03},
    {0,0,0x30,0,0x03},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}
};
#endif
// clang-format on

NEAR uint8_t pattern_array[PATTERN_COUNT][LED_LINE_LENGTH];

NEAR uint8_t ir_led_driver_buffer[PATTERN_COUNT][DRIVER_BUFFER_LENGTH];

NEAR uint8_t driver_mask[DRIVER_BUFFER_LENGTH];

void expand_array(uint8_t *buffer, uint8_t *value)
{

  uint8_t j, k;
  for (j = 0, k = 0; j < LED_LINE_LENGTH; j++)
  {
    // Process an entire byte of value, bit by bit, within this single loop body.
    // This means we will touch two bytes of buffer.

    // 0x03 is adjacent bits on: 0b00000011

    buffer[k] = 0;
    if (value[j] & 0x01)
      buffer[k] |= (0x03 << 0);
    if (value[j] & 0x02)
      buffer[k] |= (0x03 << 2);
    if (value[j] & 0x04)
      buffer[k] |= (0x03 << 4);
    if (value[j] & 0x08)
      buffer[k] |= (0x03 << 6);
    k++;
    buffer[k] = 0;
    if (value[j] & 0x10)
      buffer[k] |= (0x03 << 0);
    if (value[j] & 0x20)
      buffer[k] |= (0x03 << 2);
    if (value[j] & 0x40)
      buffer[k] |= (0x03 << 4);
    if (value[j] & 0x80)
      buffer[k] |= (0x03 << 6);
    k++;
  }
}

void line_array_init(uint8_t index, uint8_t *value) { expand_array(ir_led_driver_buffer[index], value); }

void default_array_init(void)
{
  uint8_t i;
  for (i = 0; i < PATTERN_COUNT; i++)
  {
    uint8_t j;
    for (j = 0; j < LED_LINE_LENGTH; j++)
    {
      pattern_array[i][j] = default_pattern_array[i][j];
    }
    line_array_init(i, (uint8_t *)pattern_array[i]);
  }
  {
    uint8_t mask[LED_LINE_LENGTH];
    uint8_t j;
    for (j = 0; j < LED_LINE_LENGTH; j++)
    {
      mask[j] = default_mask[j];
    }
    expand_array(driver_mask, mask);
  }
}
