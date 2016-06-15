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

#include "Config.h"

/// Enable development options
#define ENABLE_DEV

/// running patterns even when no sync arrives.
#define ENABLE_SIMULATION

/// enable the serial console - except on Cosmic, can't make it fit there.
#ifndef OSVR_IR_COSMIC_STM8
#define ENABLE_UART
#endif

/// Time (usec) it takes from the sync signal going low, to us driving nOE low
/// (with delay off) - measured with logic analyzer
#if defined(OSVR_IR_IAR_STM8)
#define SYNC_INTERRUPT_OVERHEAD 11
#elif defined(OSVR_IR_COSMIC_STM8)
// roughly 5.5 usec or less with Cosmic
#define SYNC_INTERRUPT_OVERHEAD 4
#endif

/// Time (usec) that the sync signal stays low - measured with logic analyzer,
/// but may vary between units?
#define SYNC_LOW_DURATION 236

/// Doesn't work right now?
//#define TRIGGER_ON_RISE

/// Alternate method of triggering on rise, when plain old triggering on rise doesn't work.
#define WAIT_FOR_RISE

#if defined(TRIGGER_ON_RISE) && defined(WAIT_FOR_RISE)
#error "Can't both trigger on rise and wait for rise - only one method of timing to rise can be used at once."
#endif

/// Delay (msec) at the beginning of of the flash process
//#define SYNC_DELAY_MS 0
/// Delay (usec, <1000) at the beginning of of the flash process
//#define SYNC_DELAY_US 30

/// Delay to try to jump the sync-low duration
///#define SYNC_DELAY_US (SYNC_LOW_DURATION - SYNC_INTERRUPT_OVERHEAD)

// Use a timer, rather than a delay loop, to provide sync delay. Recommended,
// but limits total delay to 2000ms.
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
#undef LOW_GAIN_MODE
#ifdef LOW_GAIN_MODE

/// These are for "low-gain" camera mode
#define FLASH_BRIGHT_PERIOD 550
/// if you make this too small, will run out of time to upload the dim pattern.
#define FLASH_INTERVAL_PERIOD 70
#define FLASH_DIM_PERIOD 200

#else

#define FLASH_BRIGHT_PERIOD 150
#define FLASH_INTERVAL_PERIOD 100
#define FLASH_DIM_PERIOD 20
#endif

/// How long to wait after the end of the LED process before accepting a new sync interrupt?
#define FLASH_SYNC_LOCKOUT_PERIOD 1000

/// Simulation timer period, milliseconds - how frequently a sync pulse will be "faked" in the absence of sync.
#define SIMULATION_PERIOD 70

/// We have a ~10ms frame, only let us schedule for 9ms of it for safety
/// This is used for an error check below only.
#define MAX_TOTAL_DURATION 9000

/// Timer value (microseconds) and thus the max value of any of the process timer periods.
#define MAX_FLASH_PERIOD 8000

#ifdef OSVR_IR_IAR_STM8
/// Offset taken (reducing timer duration) to account for computational overhead
/// when setting flash period, to achieve correct duration validated by logic analyzer
#define MAX_FLASH_PERIOD_ADJUSTMENT 14
/// Offset taken (reducing timer duration) to account for computational overhead
/// when setting blank period, to achieve correct duration validated by logic analyzer
#define MAX_BLANK_PERIOD_ADJUSTMENT 12
/// Offset taken (reducing timer duration) to account for computational overhead
/// when setting interval period, to achieve correct duration validated by logic analyzer
#define MAX_INTERVAL_PERIOD_ADJUSTMENT 12

/// Additional overhead of the startup delay timer measured to be roughly 13.78usec
#define SYNC_TIMER_DELAY_ADJUSTMENT 13

#elif defined(OSVR_IR_COSMIC_STM8)

/// Cosmic has different timings...
#define MAX_FLASH_PERIOD_ADJUSTMENT 7
#define MAX_BLANK_PERIOD_ADJUSTMENT 7
#define MAX_INTERVAL_PERIOD_ADJUSTMENT 7

#define SYNC_TIMER_DELAY_ADJUSTMENT 6

#endif

/// Check individual parameter bounds
#if FLASH_BRIGHT_PERIOD <= MAX_FLASH_PERIOD_ADJUSTMENT || FLASH_BRIGHT_PERIOD >= MAX_FLASH_PERIOD
#error "FLASH_BRIGHT_PERIOD out of range!"
#endif
#if FLASH_INTERVAL_PERIOD <= MAX_INTERVAL_PERIOD_ADJUSTMENT || FLASH_INTERVAL_PERIOD >= MAX_FLASH_PERIOD
#error "FLASH_INTERVAL_PERIOD out of range!"
#endif
#if FLASH_DIM_PERIOD <= MAX_BLANK_PERIOD_ADJUSTMENT || FLASH_DIM_PERIOD >= MAX_FLASH_PERIOD
#error "FLASH_DIM_PERIOD out of range!"
#endif

#if defined(SYNC_DELAY_TOTAL_US) && defined(SYNC_DELAY_TIMER)
#if SYNC_DELAY_TOTAL_US > MAX_FLASH_PERIOD
#error "Total sync delay exceeds maximum possible for SYNC_DELAY_TIMER mode!"
#endif
#endif

/// Check overall timing
#ifdef SYNC_DELAY_TOTAL_US
#define TOTAL_DURATION                                                                                                 \
  (SYNC_DELAY_TOTAL_US + FLASH_BRIGHT_PERIOD + 5 * (FLASH_INTERVAL_PERIOD + FLASH_DIM_PERIOD) +                        \
   FLASH_SYNC_LOCKOUT_PERIOD)
#else
#define TOTAL_DURATION                                                                                                 \
  (FLASH_BRIGHT_PERIOD + 5 * (FLASH_INTERVAL_PERIOD + FLASH_DIM_PERIOD) + FLASH_SYNC_LOCKOUT_PERIOD)
#endif

#if (TOTAL_DURATION) > MAX_TOTAL_DURATION
#error "The total timer duration exceeds the available time!"
#endif

/// Ports and pins

// PB0: IR_LED_PWR_EN active high
#define PORT_LED_PWR_EN GPIOB
#define PIN_LED_PWR_EN GPIO_PIN_0

#define PORT_LATCH GPIOC
#define PIN_LATCH GPIO_PIN_2

#define PORT_N_OE GPIOC
#define PIN_N_OE GPIO_PIN_3

#define PORT_CAMERA_SYNC GPIOD
#define PIN_CAMERA_SYNC GPIO_PIN_7
#define EXTI_CAMERA_SYNC EXTI_PORT_GPIOD
/// Port D7 is the TLI source
#define CAMERA_SYNC_IRQ_VECTOR ITC_IRQ_TLI
#define CAMERA_SYNC_IS_TLI_SOURCE

/// Level reversed on every external (sync) interrupt.
/// testpoint 7 on Sensics schematic, TP22 on HDK 1.2 schematic
#define PORT_TESTPOINT_7 GPIOD
#define PIN_TESTPOINT_7 GPIO_PIN_0

/// Level reversed on every simulation interrupt.
/// testpoint 8 on Sensics schematic, TP23 on HDK 1.2 schematic
#define PORT_TESTPOINT_8 GPIOD
#define PIN_TESTPOINT_8 GPIO_PIN_2

/// Goes high after initial pattern pulse starts, low just before it ends.
/// testpoint 9 on Sensics schematic, TP24 on HDK 1.2 schematic
#define PORT_TESTPOINT_9 GPIOD
#define PIN_TESTPOINT_9 GPIO_PIN_3

/// Goes high at the start of the process timer handler, low at the end
/// testpoint 10 on Sensics schematic, TP25 on HDK 1.2 schematic
#define PORT_TESTPOINT_10 GPIOD
#define PIN_TESTPOINT_10 GPIO_PIN_4

#endif
