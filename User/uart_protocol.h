/** @file
    @brief Header for a byte-by-byte UART protocol

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

#ifndef __UART_PROTOCOL__
#define __UART_PROTOCOL__

#include "stm8s.h" // for uint8_t without conflicts

void protocol_init();
uint8_t protocol_is_output_ready();
uint8_t protocol_get_output_byte();
void protocol_put_input_byte( uint8_t ch );

#endif