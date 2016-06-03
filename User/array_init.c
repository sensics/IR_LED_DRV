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
#ifdef OSVR_IR_IAR_STM8
#include "stm8s.h"
#endif

/* Standard includes */
/* - none - */

// clang-format off
__eeprom uint8_t default_pattern_array[PATTERN_COUNT][LED_LINE_LENGTH] =
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
// clang-format on

__tiny uint8_t pattern_array[PATTERN_COUNT][LED_LINE_LENGTH];


__tiny uint8_t ir_led_driver_buffer[PATTERN_COUNT][LED_LINE_LENGTH*2];

void line_array_init(uint8_t index, uint8_t* value)
{
  for(uint8_t j=0,k=0;j<LED_LINE_LENGTH;j++)
  {
      ir_led_driver_buffer[index][k] = 0;
      if (value[j] & 0x01) ir_led_driver_buffer[index][k] |= (0x03 <<  0) ;
      if (value[j] & 0x02) ir_led_driver_buffer[index][k] |= (0x03 <<  2) ;
      if (value[j] & 0x04) ir_led_driver_buffer[index][k] |= (0x03 <<  4) ;
      if (value[j] & 0x08) ir_led_driver_buffer[index][k] |= (0x03 <<  6) ;
      k++;
      ir_led_driver_buffer[index][k] = 0;
      if (value[j] & 0x10) ir_led_driver_buffer[index][k] |= (0x03 <<  0) ;
      if (value[j] & 0x20) ir_led_driver_buffer[index][k] |= (0x03 <<  2) ;
      if (value[j] & 0x40) ir_led_driver_buffer[index][k] |= (0x03 <<  4) ;
      if (value[j] & 0x80) ir_led_driver_buffer[index][k] |= (0x03 <<  6) ;
      k++;
  }
}

void default_array_init(void)
{
  for(uint8_t i=0;i<PATTERN_COUNT;i++)
  {
    for(uint8_t j=0;j<LED_LINE_LENGTH;j++)
      pattern_array[i][j] = default_pattern_array[i][j];
    line_array_init( i, (uint8_t*)pattern_array[i] );
  }
}
