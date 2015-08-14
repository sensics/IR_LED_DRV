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
#include "uart_protocol.h"
#include "array_init.h"

/* Library/third-party includes */
#include "stm8s.h"

/* Standard includes */
#include "intrinsics.h"  //__enable_interrupt ().


#define PIN_LATCH               GPIO_PIN_2
#define PIN_N_OE                GPIO_PIN_3
#define PIN_CAMERA_SYNC         GPIO_PIN_7
#define PIN_TESTPOINT_7         GPIO_PIN_0
#define PIN_TESTPOINT_8         GPIO_PIN_2
#define PIN_TESTPOINT_9         GPIO_PIN_3
#define PIN_TESTPOINT_10        GPIO_PIN_4


#define BIT_CRCEN       (1<<5)
#define BIT_CRCNEXT     (1<<4)

#define MCU_CLOCK           16000000

uint16_t t_light, t_dark = 0;
uint8_t index_16 = 15;

void Delay(uint16_t n)
{
 while(n--);
}

void pwm_timer(uint16_t t1, uint16_t t2)
{
   GPIO_WriteLow(GPIOC,PIN_N_OE);             
   Delay(t1);
   GPIO_WriteHigh(GPIOC,PIN_N_OE); 
   Delay(t2);
}

void SPI_SendByte(uint8_t data)
{         
  while(!(SPI->SR & SPI_SR_TXE));           
  SPI->DR = data; 
  //SPI->DR = 0xFF;
}

void Send_array_spi_data()
{
  GPIO_WriteLow(GPIOC,PIN_LATCH);     // Prepare driver latch enable for the next data latch 

  SPI_SendByte(0); // for 96 bit EVB
  SPI_SendByte(0); // for 96 bit EVB

  uint8_t* ptr = ir_led_driver_buffer[index_16];
  uint8_t k = LED_LINE_LENGTH*2;
  while(k--)     
    SPI_SendByte(*ptr++);
  
  Delay(1);
  GPIO_WriteHigh(GPIOC,PIN_LATCH);  
}

int _blankIndex = 0;
void Send_blanks_spi_data()
{
  GPIO_WriteLow(GPIOC,PIN_LATCH);     // Prepare driver latch enable for the next data latch 
  
  SPI_SendByte(0); // for 96 bit EVB
  SPI_SendByte(0); // for 96 bit EVB

    
  for( int i = 0; i < 5; i++ )
  {
    if( i == _blankIndex )
    {
      SPI_SendByte(0xFF);
      SPI_SendByte(0xFF);
    }
    else
    {
      SPI_SendByte(0);
      SPI_SendByte(0);
    }
  }
  
  Delay(1);
  GPIO_WriteHigh(GPIOC,PIN_LATCH);  
}

//  Send a message to the debug port (UART1).
//
void Printf(char *message)
{
    char *ch = message;
    while (*ch)
    {
        UART1->DR = (u8) (*ch);
        while ((UART1->SR & (u8) UART1_FLAG_TXE) == RESET);
        ch++;
    }
}

void set_flash_next_interrupt_time( uint16_t flash_time_us );

#define MAX_FLASH_PERIOD 2000

#define MAX_FLASH_PERIOD_ADJUSTMENT             20
#define MAX_BLANK_PERIOD_ADJUSTMENT             15
#define MAX_INTERVAL_PERIOD_ADJUSTMENT          15

uint16_t _flash_blank_period                 =  25;   // max 2000
uint16_t _flash_interval_period              = 125;   // max 2000
uint16_t _flash_period                       = 400;   // max 2000

uint16_t _flash_blank_period_as_timer;
uint16_t _flash_interval_period_as_timer;
uint16_t _flash_period_as_timer;

void set_flash_period( uint16_t period )
{
  _flash_period                 = period;
  _flash_period_as_timer        = MAX_FLASH_PERIOD - (_flash_period - MAX_FLASH_PERIOD_ADJUSTMENT);
}

void set_blank_period( uint16_t period )
{
  _flash_blank_period                 = period;
  _flash_blank_period_as_timer        = MAX_FLASH_PERIOD - (_flash_blank_period - MAX_BLANK_PERIOD_ADJUSTMENT);
}

void set_interval_period( uint16_t period )
{
  _flash_interval_period                 = period;
  _flash_interval_period_as_timer        = MAX_FLASH_PERIOD - (_flash_interval_period - MAX_INTERVAL_PERIOD_ADJUSTMENT);
}


uint8_t  _simulation_period                  = 70;
uint8_t update_led = 0;
uint8_t _flash_on = 0;

// can be called from anywhere - it just starts flash process
void flash_process_start()
{
    // disable external interrupt
    GPIO_Init( GPIOD, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_NO_IT );
    
    // turn-on flash
    GPIO_WriteLow(GPIOC,PIN_N_OE);
    //GPIO_WriteLow( GPIOD, PIN_TESTPOINT_10 ); 

    // process timer restart  
    //TIM1_SetCounter(0);
    //set_flash_next_interrupt_time( _flash_period-20 );
    TIM1_SetCounter( _flash_period_as_timer );
    
    // start process timer
    TIM1_Cmd(ENABLE);

    // test pulse on T9
    GPIO_WriteHigh( GPIOD, PIN_TESTPOINT_9 ); 
    
    // start blanc sequence
    _blankIndex = 0;
    _flash_on = 0;
}

void set_flash_pulse_width_naked( uint16_t flash_time_us );

// INT     -______________________
// FLASH   _---_--_--_--_--_--____
// B       ____-__-__-__-__-______   
// P       ___________________-___ 

uint8_t _update_led_blank = 0;
// called by hardware timer (process timer)
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
    GPIO_WriteHigh( GPIOD, PIN_TESTPOINT_10 ); 
  
    // turn on/off flash
    if( _flash_on )    GPIO_WriteLow(GPIOC,PIN_N_OE);
    else               GPIO_WriteHigh(GPIOC,PIN_N_OE);

    // test pulse on T9
    GPIO_WriteLow( GPIOD, PIN_TESTPOINT_9 ); 
  
    // stop timer 
    if( _flash_on  ) 
    {
      TIM1_SetCounter( _flash_blank_period_as_timer );
      
      _blankIndex++;
    }
    else
    {
      if( _blankIndex < 5 )
      {
        _update_led_blank = 1;

        TIM1_SetCounter( _flash_interval_period_as_timer );
      }
      else
      {
        TIM1_Cmd(DISABLE);
        
        TIM1_SetCounter( _flash_period_as_timer );
        
        // enable external interrrupt
        GPIO_Init( GPIOD, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_IT );
    
        // allow new pattern to be written to LEDs
        update_led = 1;
      }
    }
    _flash_on = !_flash_on;
    
    // Cleat Interrupt Pending bit
    TIM1_ClearITPendingBit( TIM1_IT_UPDATE );

    GPIO_WriteLow( GPIOD, PIN_TESTPOINT_10 ); 
}

int8_t _simulation_in_process = 0;

// called by hardware timer (simulated sync signal)
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
    // test point output
    GPIO_WriteReverse( GPIOD, PIN_TESTPOINT_8 ); 
  
    // Cleat Interrupt Pending bit
    TIM2_ClearITPendingBit( TIM2_IT_UPDATE );
    
    _simulation_in_process = 1;
    
    // start flash by simualtion (comment it out to stop simulation)
    flash_process_start();
}

// called by change on external pin (sync signal, exposure)
INTERRUPT_HANDLER(TLI_IRQHandler, 0)
{
    // test point output
    GPIO_WriteReverse( GPIOD, PIN_TESTPOINT_7 ); 
     
    // simulation timer restart  
    TIM2_SetCounter(0);
    
    _simulation_in_process = 0;
    
    // start flash by sync
    flash_process_start();
}

void set_flash_timer_max_period( uint16_t flash_time_us )
{
    disableInterrupts();
    
    TIM1_Cmd(DISABLE);
    
    TIM1_TimeBaseInit( (MCU_CLOCK / 1000000) - 1, TIM1_COUNTERMODE_UP, flash_time_us, 0);
    TIM1_ClearFlag( TIM1_FLAG_UPDATE );
    TIM1_SetCounter(0);
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE );    
    
    //TIM1_Cmd(ENABLE);
    enableInterrupts();
}

//void set_flash_next_interrupt_time( uint16_t flash_time_us )
//{
//      TIM1_SetCounter(MAX_FLASH_PERIOD - flash_time_us);
//}

void set_interval_simulator( uint8_t simulation_period_time_ms )
{
    disableInterrupts();
    
    TIM2_Cmd(DISABLE);
    
//    #define SIM_TIME     70 // in milliseconds
//    #define TIM2_PERIOD  ((MCU_CLOCK / 128) * SIM_TIME / 1000)
//    TIM2_TimeBaseInit( TIM2_PRESCALER_128, TIM2_PERIOD );
    
    TIM2_TimeBaseInit( TIM2_PRESCALER_128, (MCU_CLOCK / 128) * simulation_period_time_ms / 1000 );
    TIM2_ClearFlag( TIM2_FLAG_UPDATE );
    TIM2_ITConfig( TIM2_IT_UPDATE, ENABLE );
    
    TIM2_Cmd(ENABLE);

    enableInterrupts();
}

void main(void)
{
    CLK_DeInit();  
    CLK_HSICmd(ENABLE); // Internal 16 MHz clock enable
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
    CLK->SWR = 0xB4;
    CLK_CCOCmd(ENABLE);
  
    GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_HIGH_SLOW); // IR_LED_PWR_EN active high
    UART1_Init(115200, UART1_WORDLENGTH_8D,UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE); // UART1 init
    GPIO_Init(GPIOC,PIN_LATCH,GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(GPIOC,PIN_N_OE,GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_SLOW);
  //GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_SLOW);
  //SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07); //  SPI Init
  //SPI->CR2 &= ~(BIT_CRCEN|BIT_CRCNEXT);
    SPI_Cmd(ENABLE);

    // Init test points outputs
    GPIO_DeInit(GPIOD);
    GPIO_Init( GPIOD, PIN_TESTPOINT_7,  GPIO_MODE_OUT_PP_LOW_SLOW );
    GPIO_Init( GPIOD, PIN_TESTPOINT_8,  GPIO_MODE_OUT_PP_LOW_SLOW );
    GPIO_Init( GPIOD, PIN_TESTPOINT_9,  GPIO_MODE_OUT_PP_LOW_SLOW );
    GPIO_Init( GPIOD, PIN_TESTPOINT_10, GPIO_MODE_OUT_PP_HIGH_SLOW );
    
    // Init external IRQ
    GPIO_Init( GPIOD, PIN_CAMERA_SYNC, GPIO_MODE_IN_FL_IT );
    EXTI_SetExtIntSensitivity( EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY );
    EXTI_SetTLISensitivity( EXTI_TLISENSITIVITY_FALL_ONLY );

    // timer initialization (simulation timer)
    TIM1_DeInit();
    TIM2_DeInit();

    // enable interrupts
    enableInterrupts();

    protocol_init();

    default_array_init();
    
    set_flash_period( _flash_period );
    set_blank_period( _flash_blank_period );
    set_interval_period( _flash_interval_period );
    
    set_flash_timer_max_period( MAX_FLASH_PERIOD );
    TIM1_SetCounter(_flash_period_as_timer);
    
    set_interval_simulator(_simulation_period);
    
    index_16 = 0;
    
    while (1)
    {  
      if(_update_led_blank)
      {
          _update_led_blank = 0;
          
          Send_blanks_spi_data();
      }
      
      if( update_led )
      {
        update_led = 0;
        
        if( _simulation_in_process )
          Send_blanks_spi_data();
        else
          Send_array_spi_data(); // Serialize 80 (96) bit for IR LED's drivers

        index_16++;
        index_16 &= 0x0F;
      }

      /*
      if( UART1_GetFlagStatus(UART1_FLAG_RXNE) == SET )
      {
          uint8_t ch = UART1_ReceiveData8();
           
          if( UART1_GetFlagStatus(UART1_FLAG_TXE) == SET )
            UART1_SendData8( ch );
      }
      */
     
      if( UART1_GetFlagStatus(UART1_FLAG_RXNE) == SET )
          protocol_put_input_byte( UART1_ReceiveData8() );
           
      if( UART1_GetFlagStatus(UART1_FLAG_TXE) == SET && protocol_is_output_ready() )
          UART1_SendData8( protocol_get_output_byte() );
    }
}

void assert_failed(u8* file, u32 line)
{ while (1) {} }