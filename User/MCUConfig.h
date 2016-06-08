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

#ifndef INCLUDED_MCUConfig_h_GUID_D1C232A1_AC37_47A3_347A_DFA3E6FE70CA
#define INCLUDED_MCUConfig_h_GUID_D1C232A1_AC37_47A3_347A_DFA3E6FE70CA

/// running patterns even when no sync arrives.
#define ENABLE_SIMULATION

/// Time (usec) it takes from the sync signal going low, to us driving nOE low (with delay off) - measured with logic
/// analyzer
#define SYNC_INTERRUPT_OVERHEAD 23

/// Time (usec) that the sync signal stays low - measured with logic analyzer, but may vary between units?
#define SYNC_LOW_DURATION 233

/// Delay (msec) at the beginning of of the flash process
//#define SYNC_DELAY_MS 0
/// Delay (usec, <1000) at the beginning of of the flash process
//#define SYNC_DELAY_US 30

#define SYNC_DELAY_US (SYNC_LOW_DURATION - SYNC_INTERRUPT_OVERHEAD)

// Use a timer, rather than a delay loop, to provide sync delay. Recommended, but limits total delay to 2000ms.
#define SYNC_DELAY_TIMER

#if defined(SYNC_DELAY_MS) && defined(SYNC_DELAY_US)
#define SYNC_DELAY_TOTAL_US (SYNC_DELAY_US + SYNC_DELAY_MS * 1000)
#elif defined(SYNC_DELAY_MS)
#define SYNC_DELAY_TOTAL_US (SYNC_DELAY_MS * 1000)
#elif defined(SYNC_DELAY_US)
#define SYNC_DELAY_TOTAL_US (SYNC_DELAY_US)
#endif

/// For testing, only run the process every SYNC_INTERVAL sync pulses.
//#define SYNC_INTERVAL 3

#ifdef SYNC_INTERVAL
/// Can't have SYNC_INTERVAL and the SIMULATION enabled at once.
#undef ENABLE_SIMULATION
#endif

/// Periods in microseconds.
#define FLASH_BRIGHT_PERIOD 250
/// if you make this too small, will run out of time to upload the dim pattern.
#define FLASH_INTERVAL_PERIOD 100
#define FLASH_DIM_PERIOD 25

/// Simulation timer period, unknown units.
#define SIMULATION_PERIOD 70

#endif
