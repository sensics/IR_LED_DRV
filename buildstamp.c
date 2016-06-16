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
// All rights reserved.
*/

/* Internal Includes */
#include "User/MCUConfig.h"

#ifndef SYNC_DELAY_TOTAL_US
#define SYNC_DELAY_TOTAL_US 0
#endif

#define S_IMPL(X) #X
#define S(X) S_IMPL(X)
const char DESC[] =
    "Sensics IR_LED_DRV "
#ifdef PRODUCTION
    "(PRODUCTION)"
#else
    "(DEV BUILD)"
#endif
    " Built " __DATE__ " " __TIME__ ".Delay:" S(SYNC_DELAY_TOTAL_US) ",Bright:" S(FLASH_BRIGHT_PERIOD) ",Interval:" S(
        FLASH_INTERVAL_PERIOD) ",Dim:" S(FLASH_DIM_PERIOD) ",Lockout:" S(FLASH_SYNC_LOCKOUT_PERIOD);
