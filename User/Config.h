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
// All rights reserved.
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

#include "stm8s.h" // for uint8_t without conflicts

#elif defined(__CSMC__)
// On Cosmic C compiler
/// @todo detect target, not just compiler
#define OSVR_IR_EMBEDDED
#define OSVR_IR_COSMIC_STM8

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
