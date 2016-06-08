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

/// Enable development options
#define ENABLE_DEV

/// running patterns even when no sync arrives.
#define ENABLE_SIMULATION

/// enable the serial console
#define ENABLE_UART

/// Time (usec) it takes from the sync signal going low, to us driving nOE low
/// (with delay off) - measured with logic analyzer
#define SYNC_INTERRUPT_OVERHEAD 11

/// Time (usec) that the sync signal stays low - measured with logic analyzer,
/// but may vary between units?
#define SYNC_LOW_DURATION 233

#define TRIGGER_ON_RISE

/// Delay (msec) at the beginning of of the flash process
//#define SYNC_DELAY_MS 0
/// Delay (usec, <1000) at the beginning of of the flash process
//#define SYNC_DELAY_US 30

#define SYNC_DELAY_US (SYNC_LOW_DURATION - SYNC_INTERRUPT_OVERHEAD)

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
#define FLASH_BRIGHT_PERIOD 250
/// if you make this too small, will run out of time to upload the dim pattern.
#define FLASH_INTERVAL_PERIOD 100
#define FLASH_DIM_PERIOD 25

/// Simulation timer period, unknown units.
#define SIMULATION_PERIOD 70

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
