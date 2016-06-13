/** @file
    @brief Main implementation

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
#include "MCUConfig.h"

#include "array_init.h"
#include "uart_protocol.h"

/* Library/third-party includes */
#include "stm8s.h"
#include "stm8s_it.h"

/* Standard includes */
#ifdef OSVR_IR_IAR_STM8
#include "intrinsics.h" //__enable_interrupt ().
#endif

/// Warnings related to dev and production.
#ifdef ENABLE_DEV
// If not in IAR, must skip this warning, since we might actually be in GCC generating input for ctags in ST Visual Develop,
// and there's no way to distinguish the two environments :-/
#ifdef OSVR_IR_IAR_STM8
#warning "Development build!"
#endif

#else
// This should be a production build.
#ifdef SYNC_INTERVAL
#error "SYNC_INTERVAL cannot be defined in a production build!"
#endif
/// @todo is this the policy we want?
#ifndef ENABLE_SIMULATION
#error "ENABLE_SIMULATION must be defined in a production build!"
#endif
#endif

#define MCU_CLOCK 16000000
uint8_t index_16 = 15;

/// @name Timer 1 values
/// @brief Units are microseconds. Total time for a flash process, roughly
/// _flash_period + 5 * _flash_blank_period + 5 *
/// _flash_interval_period
/// @{

uint16_t _flash_blank_period    = FLASH_DIM_PERIOD;      // max 2000
uint16_t _flash_interval_period = FLASH_INTERVAL_PERIOD; // max 2000
uint16_t _flash_period          = FLASH_BRIGHT_PERIOD;   // max 2000

/// @}

static void Delay(uint16_t n)
{
  while (n--)
    ;
}

/// USEC_TO_TICKS
/// Based on how long the following loop is for each compiler:
/// while (n--) {}
#if defined(OSVR_IR_IAR_STM8)
// this loop is ldw, decw, tnzw, jrne on IAR - so 5 (5.5?) cycles.
#define USEC_TO_TICKS(USEC) (((unsigned short)((MCU_CLOCK * (unsigned long)USEC) / 1000000UL) - 5) / 5)
#elif defined(OSVR_IR_COSMIC_STM8)
// this loop is ldw, decw, ldw, incw, jrne on Cosmic - so 6 (6.5?) cycles.
#define USEC_TO_TICKS(USEC) (((unsigned short)((MCU_CLOCK * (unsigned long)USEC) / 1000000UL) - 6) / 6)
#else
#error unported to this compiler
#endif

static inline void delay_ticks(unsigned short ticks)
{
  while (ticks--)
  {
  }
}
#ifdef __ICCSTM8__
// suppress "never referenced" diagnostic for these delay functions - they get used depending on the config.
#pragma diag_suppress = Pe177
#endif // __ICCSTM8__
static void delay_us(unsigned short usec) { delay_ticks(USEC_TO_TICKS(usec)); }
static void delay_ms(unsigned short msec)
{
  do
  {
    delay_ticks(USEC_TO_TICKS(1000));
  } while (--msec);
}
#ifdef __ICCSTM8__
// restore diagnostic state
#pragma diag_default = Pe177
#endif // __ICCSTM8__

void SPI_WaitForTransmissionToComplete();

static inline void SPI_WaitForTransmissionToComplete()
{
  while (!(SPI->SR & SPI_SR_TXE))
  {
  }
}

static void SPI_SendByte(uint8_t data)
{
  SPI_WaitForTransmissionToComplete();
  SPI->DR = data;
}

static void Send_array_spi_data()
{
  GPIO_WriteLow(PORT_LATCH, PIN_LATCH); // Prepare driver latch enable for the next data latch

  SPI_SendByte((uint8_t)0x00); // for 96 bit EVB
  SPI_SendByte((uint8_t)0x00); // for 96 bit EVB

  uint8_t *ptr = ir_led_driver_buffer[index_16];
  uint8_t k    = DRIVER_BUFFER_LENGTH;
  while (k)
  {
    k--;
    SPI_SendByte(*ptr);
    ptr++;
  }

  /// Wait for transmission to complete.
  SPI_WaitForTransmissionToComplete();
  Delay(1);
  GPIO_WriteHigh(PORT_LATCH, PIN_LATCH);
}

typedef enum {
  /// Pattern shifted into LEDs, ready to display
  STATE_PROCESS_AWAITING_START,
  STATE_IN_STARTUP_DELAY,
  /// Currently displaying LEDs as indicated by the pattern
  STATE_PATTERN_ON,
  /// the following 3 have substates, indicated by _subState, for each of
  /// LED_LINE_LENGTH, since dim illumination
  /// happens in byte-sized blocks.
  STATE_BETWEEN_PULSES_AWAITING_BLANK_UPLOAD,
  STATE_BETWEEN_PULSES_AWAITING_TIMER,
  /// All off except a single byte of LEDs, for dim illumination
  STATE_DIM_PULSE_ON,
  /// All dim illumination cycles completed, awaiting pattern illumination.
  STATE_AWAITING_PATTERN
} State_t;

State_t _procState = STATE_PROCESS_AWAITING_START;

uint8_t _subState = 0;

static void Send_blanks_spi_data()
{
  GPIO_WriteLow(PORT_LATCH, PIN_LATCH); // Prepare driver latch enable for the next data latch

  SPI_SendByte((uint8_t)0x00); // for 96 bit EVB
  SPI_SendByte((uint8_t)0x00); // for 96 bit EVB
  int i;
  for (i = 0; i < LED_LINE_LENGTH; i++)
  {
    if (i == _subState)
    {
      /// @todo For one "blank" interval per process, each LED is illuminated,
      /// to provide "dim" - is this correct understanding?
      SPI_SendByte((uint8_t)0xFF);
      SPI_SendByte((uint8_t)0xFF);
    }
    else
    {
      SPI_SendByte((uint8_t)0x00);
      SPI_SendByte((uint8_t)0x00);
    }
  }

  SPI_WaitForTransmissionToComplete();
  Delay(1);
  GPIO_WriteHigh(PORT_LATCH, PIN_LATCH);
}

uint16_t _flash_blank_period_as_timer;
uint16_t _flash_interval_period_as_timer;
uint16_t _flash_period_as_timer;

/// Set duration (starting from "start" or "sync signal" starting flash process)
/// of initial (pattern-based) LED flash
/// pulse. Essentially, the "bright" pulse duration.
void set_flash_period(uint16_t period)
{
  _flash_period          = period;
  _flash_period_as_timer = MAX_FLASH_PERIOD - (_flash_period - MAX_FLASH_PERIOD_ADJUSTMENT);
}

/// Set time from LEDs on to LEDs off between a pair of blanks. This is for the
/// "blank pattern" which has all but one
/// byte of LEDs off. Essentially, this is the "dim" duration.
void set_blank_period(uint16_t period)
{
  _flash_blank_period          = period;
  _flash_blank_period_as_timer = MAX_FLASH_PERIOD - (_flash_blank_period - MAX_BLANK_PERIOD_ADJUSTMENT);
}

/// Set time between LEDs on during flash process.
void set_interval_period(uint16_t period)
{
  _flash_interval_period          = period;
  _flash_interval_period_as_timer = MAX_FLASH_PERIOD - (_flash_interval_period - MAX_INTERVAL_PERIOD_ADJUSTMENT);
}

void actuallyStartFlashProcess();

static inline void actuallyStartFlashProcess()
{
  // turn-on flash
  GPIO_WriteLow(PORT_N_OE, PIN_N_OE);
  // GPIO_WriteLow( GPIOD, PIN_TESTPOINT_10 );

  // process timer restart
  TIM1_SetCounter(_flash_period_as_timer);

  // start process timer
  TIM1_Cmd(ENABLE);

  // test pulse on T9
  GPIO_WriteHigh(PORT_TESTPOINT_9, PIN_TESTPOINT_9);

  // start blank sequence
  _subState  = 0;
  _procState = STATE_PATTERN_ON;
}

void enable_sync_interrupt();
/// Requires that you've already used GPIO_Init to do the overall, full pin setup: this just flips one bit to enable the
/// external interrupt, which is much (over 10usec) faster.
static inline void enable_sync_interrupt()
{
  // Set pin's bit in CR2 register
  PORT_CAMERA_SYNC->CR2 |= (uint8_t)PIN_CAMERA_SYNC;
}

void disable_sync_interrupt();

/// Requires that you've already used GPIO_Init to do the overall, full pin setup: this just flips one bit to disable
/// the external interrupt, which is much (over 10usec) faster.
static inline void disable_sync_interrupt()
{
  // Clear pin's bit in CR2 register
  PORT_CAMERA_SYNC->CR2 &= ~((uint8_t)PIN_CAMERA_SYNC);
}

// can be called from anywhere - it just starts flash process
static void flash_process_start()
{
  // disable external interrupt
  // GPIO_Init(PORT_CAMERA_SYNC, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_NO_IT);
  disable_sync_interrupt();

// Wait a bit before starting the flash process to account for sync mistiming.

#ifdef SYNC_DELAY_TOTAL_US
#ifdef SYNC_DELAY_TIMER
  _procState = STATE_IN_STARTUP_DELAY;
  // Set timer for delay duration.
  TIM1_SetCounter(MAX_FLASH_PERIOD - (SYNC_DELAY_TOTAL_US - SYNC_TIMER_DELAY_ADJUSTMENT));

  // start process timer
  TIM1_Cmd(ENABLE);

#else // SYNC_DELAY_TIMER ^ / v sync delay via loops

#ifdef SYNC_DELAY_MS
  delay_ms(SYNC_DELAY_MS);
#endif // SYNC_DELAY_MS

#ifdef SYNC_DELAY_US
  delay_us(SYNC_DELAY_US);
#endif // SYNC_DELAY_US

  // Proceed directly to flash process after delay loops.
  actuallyStartFlashProcess();

#endif // SYNC_DELAY_TIMER

#else // SYNC_DELAY_TOTAL_US ^ / v no sync delay.

  actuallyStartFlashProcess();

#endif
}

// INT     -______________________
// FLASH   _---_--_--_--_--_--____
// B       ____-__-__-__-__-______
// P       ___________________-___

// called by hardware timer (process timer)
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, ITC_IRQ_TIM1_OVF)
{
  // Pull test point high to signal entry into the process timer interrupt handler.
  GPIO_WriteHigh(PORT_TESTPOINT_10, PIN_TESTPOINT_10);

  // Disable this timer to avoid counting while we set it.
  // TIM1_Cmd(DISABLE);

  switch (_procState)
  {
#if defined(SYNC_DELAY_TOTAL_US) && defined(SYNC_DELAY_TIMER)
  case STATE_IN_STARTUP_DELAY:
    actuallyStartFlashProcess();
    break;
#endif // defined(SYNC_DELAY_TOTAL_US) && defined(SYNC_DELAY_TIMER)
  case STATE_PATTERN_ON:
    // end test pulse on T9
    GPIO_WriteLow(PORT_TESTPOINT_9, PIN_TESTPOINT_9);
  /// then fall through to turn off the flash, prepare for upload, etc.
  case STATE_DIM_PULSE_ON:
  {

    /// If starting from a dim state, we increment the sub state first.
    uint8_t nextSubState = _procState == STATE_DIM_PULSE_ON ? _subState + 1 : _subState;
    // turn off flash
    GPIO_WriteHigh(PORT_N_OE, PIN_N_OE);
    if (nextSubState < LED_LINE_LENGTH)
    {
      _subState  = nextSubState;
      _procState = STATE_BETWEEN_PULSES_AWAITING_BLANK_UPLOAD;
      TIM1_SetCounter(_flash_interval_period_as_timer);
    }
    else
    {
      // Disable this timer
      TIM1_Cmd(DISABLE);

      // Next time timer is enabled, use flash period as counter.
      TIM1_SetCounter(_flash_period_as_timer);

      // enable external interrupt on sync pin (floating)
      // GPIO_Init(PORT_CAMERA_SYNC, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_IT);
      enable_sync_interrupt();

      // allow new pattern to be written to LEDs
      _procState = STATE_AWAITING_PATTERN;
      _subState  = 0;
    }
  }
  break;
  case STATE_BETWEEN_PULSES_AWAITING_BLANK_UPLOAD:
// shouldn't get here!
// it means we couldn't get around to uploading the pattern before the timer went off
#ifdef ENABLE_DEV
    assert_param(_procState != STATE_BETWEEN_PULSES_AWAITING_BLANK_UPLOAD);
#else
    // Wait some more.
    TIM1_SetCounter(_flash_interval_period_as_timer);
#endif
    break;
  case STATE_BETWEEN_PULSES_AWAITING_TIMER:
    // turn on flash
    GPIO_WriteLow(PORT_N_OE, PIN_N_OE);
    TIM1_SetCounter(_flash_blank_period_as_timer);
    _procState = STATE_DIM_PULSE_ON;
    break;
  }

  // Clear Interrupt Pending bit since we handled it.
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
#if 0
  if (_procState != STATE_AWAITING_PATTERN)
  {
    // If we haven't finished the process, re-start the timer.
    TIM1_Cmd(ENABLE);
  }
#endif

  // Bring test point low to signal completion of the process timer interrupt handler.
  GPIO_WriteLow(PORT_TESTPOINT_10, PIN_TESTPOINT_10);
}

uint8_t _simulation_period = SIMULATION_PERIOD;

#ifdef ENABLE_SIMULATION

static int8_t _simulation_in_process = 0;
// called by hardware timer (simulated sync signal)
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, ITC_IRQ_TIM2_OVF)
{
  // test point output
  GPIO_WriteReverse(PORT_TESTPOINT_8, PIN_TESTPOINT_8);

  // Cleat Interrupt Pending bit
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);

  _simulation_in_process = 1;

  // start flash by simualtion (comment it out to stop simulation)
  flash_process_start();
}

#endif // ENABLE_SIMULATION

#ifdef SYNC_INTERVAL
uint8_t gotSync = 0;
#endif

// called by sync signal
INTERRUPT_HANDLER(TLI_IRQHandler, CAMERA_SYNC_IRQ_VECTOR)
{
  // test point output
  GPIO_WriteReverse(PORT_TESTPOINT_7, PIN_TESTPOINT_7);

#ifdef ENABLE_SIMULATION
  // simulation timer restart
  TIM2_SetCounter(0);

  _simulation_in_process = 0;
#endif // ENABLE_SIMULATION

#ifdef SYNC_INTERVAL
  // For debugging, only start up the flash every SYNC_INTERVAL pulses
  if (gotSync == (SYNC_INTERVAL - 1))
  {
    gotSync = 0;
    // start flash by sync
    flash_process_start();
  }
  else
  {
    gotSync++;
  }
#else
  // start flash by sync
  flash_process_start();
#endif
}

static void set_flash_timer_max_period(uint16_t flash_time_us)
{
  disableInterrupts();

  TIM1_Cmd(DISABLE);

  // prescaler is set to 15 (+1), so that the counter clock frequency is 1MHz
  // (That is, the units are microseconds)
  TIM1_TimeBaseInit((MCU_CLOCK / 1000000) - 1, TIM1_COUNTERMODE_UP, flash_time_us, 0);
  TIM1_ClearFlag(TIM1_FLAG_UPDATE);
  TIM1_SetCounter(0);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);

  // TIM1_Cmd(ENABLE);
  enableInterrupts();
}

// void set_flash_next_interrupt_time( uint16_t flash_time_us )
//{
//      TIM1_SetCounter(MAX_FLASH_PERIOD - flash_time_us);
//}

void set_interval_simulator(uint8_t simulation_period_time_ms)
{
#ifdef ENABLE_SIMULATION
  disableInterrupts();

  TIM2_Cmd(DISABLE);

  //    #define SIM_TIME     70 // in milliseconds
  //    #define TIM2_PERIOD  ((MCU_CLOCK / 128) * SIM_TIME / 1000)
  //    TIM2_TimeBaseInit( TIM2_PRESCALER_128, TIM2_PERIOD );

  TIM2_TimeBaseInit(TIM2_PRESCALER_128, (MCU_CLOCK / 128) * simulation_period_time_ms / 1000);
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

  TIM2_Cmd(ENABLE);
  enableInterrupts();
#endif // ENABLE_SIMULATION
}

void main(void)
{
  CLK_DeInit();
  GPIO_DeInit(GPIOD);
  EXTI_DeInit();

  // timer initialization (simulation timer)
  TIM1_DeInit();
  TIM2_DeInit();

  CLK_HSICmd(ENABLE); // Internal 16 MHz clock enable
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  /// switch clock source to high speed external
  CLK->SWR = 0xB4;
  CLK_CCOCmd(ENABLE);

  GPIO_Init(PORT_LED_PWR_EN, PIN_LED_PWR_EN, GPIO_MODE_OUT_PP_HIGH_SLOW); // PB0: IR_LED_PWR_EN active high

#ifdef ENABLE_UART
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE); // UART1 init
#endif

  GPIO_Init(PORT_LATCH, PIN_LATCH, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(PORT_N_OE, PIN_N_OE, GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW); // "DATA1" on HDK 1.2 schematics
  // GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_SLOW); // "DATA0" on HDK
  // 1.2 schematics - but shares a pin with SDO

  // SPI_DeInit();
  /// Setting the pin settings properly for SPI
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);     // SPI_SCK
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);     // SPI_MOSI
  GPIO_Init(PORT_LATCH, PIN_LATCH, GPIO_MODE_OUT_PP_HIGH_FAST); // enable/latch
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE,
           SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT,
           0x07); //  SPI Init
  // SPI->CR2 &= ~(BIT_CRCEN|BIT_CRCNEXT);
  SPI_Cmd(ENABLE);

  // Init test points outputs on port D
  GPIO_Init(PORT_TESTPOINT_7, PIN_TESTPOINT_7, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(PORT_TESTPOINT_8, PIN_TESTPOINT_8, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(PORT_TESTPOINT_9, PIN_TESTPOINT_9, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(PORT_TESTPOINT_10, PIN_TESTPOINT_10, GPIO_MODE_OUT_PP_HIGH_SLOW);

  // Init external IRQ
  GPIO_Init(PORT_CAMERA_SYNC, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_IT);
  // GPIO_Init(PORT_CAMERA_SYNC, PIN_CAMERA_SYNC, /*GPIO_MODE_IN_FL_IT*/ GPIO_MODE_IN_PU_IT);
  // GPIO_ExternalPullUpConfig(PORT_CAMERA_SYNC, PIN_CAMERA_SYNC, ENABLE);
  disableInterrupts();
#ifdef TRIGGER_ON_RISE

#ifdef CAMERA_SYNC_IS_TLI_SOURCE
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);
#else
  EXTI_SetExtIntSensitivity(EXTI_CAMERA_SYNC, EXTI_SENSITIVITY_RISE_ONLY);
#endif // CAMERA_SYNC_IS_TLI_SOURCE

#else

#ifdef CAMERA_SYNC_IS_TLI_SOURCE
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);
#else
  EXTI_SetExtIntSensitivity(EXTI_CAMERA_SYNC, EXTI_SENSITIVITY_FALL_ONLY);
#endif // CAMERA_SYNC_IS_TLI_SOURCE

#endif // TRIGGER_ON_RISE

#ifdef ENABLE_UART
  protocol_init();
#endif

  default_array_init();

  set_flash_period(_flash_period);
  set_blank_period(_flash_blank_period);
  set_interval_period(_flash_interval_period);

  set_flash_timer_max_period(MAX_FLASH_PERIOD);
  TIM1_SetCounter(_flash_period_as_timer);

  set_interval_simulator(_simulation_period);

  index_16 = 0;

  // enable interrupts
  enableInterrupts();

  while (1)
  {
    switch (_procState)
    {
    case STATE_BETWEEN_PULSES_AWAITING_BLANK_UPLOAD:
      Send_blanks_spi_data();
      _procState = STATE_BETWEEN_PULSES_AWAITING_TIMER;
      break;
    case STATE_AWAITING_PATTERN:
// Move to the next value in the patterns
#ifdef ENABLE_SIMULATION
      if (_simulation_in_process)
        Send_blanks_spi_data();
      else
#endif
        Send_array_spi_data(); // Serialize 80 (96) bit for IR LED's drivers

      // this is effectively index_16 = (index_16 + 1) % PATTERN_COUNT
      index_16++;
      index_16 &= 0x0F;
      _procState = STATE_PROCESS_AWAITING_START;
      break;
    }

#ifdef ENABLE_UART
    if (UART1_GetFlagStatus(UART1_FLAG_RXNE) == SET)
      protocol_put_input_byte(UART1_ReceiveData8());

    if (UART1_GetFlagStatus(UART1_FLAG_TXE) == SET && protocol_is_output_ready())
      UART1_SendData8(protocol_get_output_byte());
#endif
  }
}

void assert_failed(u8 *file, u32 line)
{
  while (1)
  {
  }
}
