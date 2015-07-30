#include "stm8s.h"

#include "uart_protocol.h"
#include "array_init.h"

enum
{
  UART_COMMAND_NONE        = 0,
  UART_COMMAND_FLASH       = 'F',
  UART_COMMAND_BLANK       = 'B',
  UART_COMMAND_INTERVAL    = 'I',
  UART_COMMAND_SIMULATION  = 'S',
  UART_COMMAND_PATTERN     = 'P',
  UART_COMMAND_ERROR       = 'E',
  UART_COMMAND_HELP        = 'H',
};

enum
{
  UART_CHARACTER_DELIMITER  = ':',
  UART_CHARACTER_EOL        = '\r',
  UART_CHARACTER_NEWLINE    = '\n',
  UART_CHARACTER_COMMA      = ',',
};

enum
{
  UART_MODE_READ  = 'R',
  UART_MODE_WRITE = 'W',
};

//FW:10000\n\r
//FR\n\r
//PW:A:00,01,02,03,04

struct UART_COMMAND
{
  uint8_t write_mode;
  
  union 
  {
    uint16_t flash_time;      // in us
    uint8_t  simulation_time; // in ms
    struct
    {
      uint8_t  index;
      uint8_t  pattern[LED_LINE_LENGTH];
    };
  };
};

#define UART_MAX_LINE_LENGTH 32
//UART_COMMAND _protocol_data = {0};
uint8_t      _protocol_line[UART_MAX_LINE_LENGTH];
uint8_t      _protocol_length = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UART_MAX_WRITE_LENGTH           256
#define UART_MAX_WRITE_LENGTH_MASK      (UART_MAX_WRITE_LENGTH-1)
typedef struct UART_WRITE_BUFFER_
{
  uint8_t      buffer[UART_MAX_WRITE_LENGTH];
  uint8_t      count;
  uint8_t      write;
  uint8_t      read;
}UART_WRITE_BUFFER;

UART_WRITE_BUFFER _write_buffer = {0};

void protocol_init()
{
  _write_buffer.count = 0;
  _write_buffer.write = 0;
  _write_buffer.read = 0;
  
  _protocol_length = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t protocol_is_output_ready()
{
  return _write_buffer.count > 0;
}

uint8_t protocol_get_output_byte()
{
  uint8_t ch = _write_buffer.buffer[_write_buffer.read++];
  _write_buffer.read &= UART_MAX_WRITE_LENGTH_MASK;
  _write_buffer.count--;
  return ch;
}

void protocol_put_output_byte( uint8_t ch )
{
  _write_buffer.buffer[_write_buffer.write++] = ch;
  _write_buffer.write &= UART_MAX_WRITE_LENGTH_MASK;
  _write_buffer.count++;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_flash_read();
void protocol_parse_flash_write();

void protocol_parse_blank_read();
void protocol_parse_blank_write();

void protocol_parse_interval_read();
void protocol_parse_interval_write();

void protocol_parse_sim_read();
void protocol_parse_sim_write();

void protocol_parse_pattern_read();
void protocol_parse_pattern_write();

void protocol_output_error( uint8_t* info, uint8_t info_length );

  void protocol_help();
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_put_input_byte( uint8_t ch )
{
  protocol_put_output_byte(ch);
  
  if( _protocol_length < UART_MAX_LINE_LENGTH )
    _protocol_line[_protocol_length++] = ch;
  
  if( ch != UART_CHARACTER_EOL )
    return;
  
  if( _protocol_length < 2 )
  {
    _protocol_length = 0;
    return;
  }

  uint8_t read;
  switch( _protocol_line[1] )
  {
  case UART_MODE_READ:  
    read = TRUE;  
    break;
    
  case UART_MODE_WRITE: 
    read = FALSE; 
    break;
    
  default:              
    protocol_output_error( "mode", 4 ); 
    _protocol_length = 0;
    return;
  }

  switch( _protocol_line[0] )
  {
  case UART_COMMAND_HELP: 
    protocol_help(); 
    break;
  case UART_COMMAND_FLASH: 
    if( read ) protocol_parse_flash_read(); 
    else       protocol_parse_flash_write(); 
    break;
  case UART_COMMAND_BLANK: 
    if( read ) protocol_parse_blank_read(); 
    else       protocol_parse_blank_write(); 
    break;
  case UART_COMMAND_INTERVAL: 
    if( read ) protocol_parse_interval_read(); 
    else       protocol_parse_interval_write(); 
    break;
  case UART_COMMAND_SIMULATION:
    if( read ) protocol_parse_sim_read(); 
    else       protocol_parse_sim_write(); 
    break;
  case UART_COMMAND_PATTERN:
    if( read ) protocol_parse_pattern_read(); 
    else       protocol_parse_pattern_write(); 
    break;
  default:              
    protocol_output_error( "command", 7 ); 
    _protocol_length = 0;
    return;
  }
  
  _protocol_length = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_output_error( uint8_t* info, uint8_t info_length )
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < info_length + 3 ) // "E:xxxxx\n"
   return;
  
  protocol_put_output_byte( UART_COMMAND_ERROR );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );

  while( info_length-- )
    protocol_put_output_byte( *info++ );

  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}
 
void protocol_output_string( uint8_t* info, uint8_t info_length )
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < info_length ) // "E:xxxxx\n"
   return;
  
  while( info_length-- )
    protocol_put_output_byte( *info++ );
}

  
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t _hex_table[] = "0123456789ABCDEF";

uint8_t hex_to_int( uint8_t ch )
{
  if( ch >= '0' && ch <= '9' ) return ch - '0';
  if( ch >= 'A' && ch <= 'F' ) return ch - 'A' + 10;
  if( ch >= 'a' && ch <= 'f' ) return ch - 'a' + 10;
  return 255;
}

extern uint8_t  _simulation_period;
extern uint16_t _flash_period;
extern uint16_t _flash_blank_period;
extern uint16_t _flash_interval_period;

void set_flash_period( uint16_t period );
void set_blank_period( uint16_t period );
void set_interval_period( uint16_t period );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_flash_pulse_width( uint16_t flash_time_us );
void set_interval_simulator( uint8_t simulation_period_time_ms );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_flash_write()
{
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t value_12 = hex_to_int(_protocol_line[3]);
  uint8_t value_08 = hex_to_int(_protocol_line[4]);
  uint8_t value_04 = hex_to_int(_protocol_line[5]);
  uint8_t value_00 = hex_to_int(_protocol_line[6]);
  if( value_12 < 0 || value_12 > 15 ||
      value_08 < 0 || value_08 > 15 || 
      value_04 < 0 || value_04 > 15 ||
      value_00 < 0 || value_00 > 15 )
  {
    protocol_output_error( "value", 5 ); 
    return;
  }
  
  uint16_t value = (((uint16_t)value_12) << 12) |
                  (((uint16_t)value_08) << 8) |
                  (((uint16_t)value_04) << 4) |
                  (((uint16_t)value_00) << 0);

  if( value < 10 || value > 10000 )
  {
    protocol_output_error( "limit", 5 ); 
    return;
  }
  
 
  //_flash_period = value;
  //_flash_period_as_timer          = MAX_FLASH_PERIOD - _flash_period - MAX_FLASH_PERIOD_ADJUSTMENT;
    set_flash_period( value );
//  set_flash_pulse_width(_flash_period);
 
  protocol_parse_flash_read();
}

void protocol_parse_flash_read()
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < 8 ) // "FR:0010\n"
   return;

  protocol_put_output_byte( UART_COMMAND_FLASH );
  protocol_put_output_byte( UART_MODE_READ );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  protocol_put_output_byte( _hex_table[(_flash_period >> 12) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_period >>  8) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_period >>  4) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_period >>  0) & 0x0F] );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_blank_write()
{
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t value_12 = hex_to_int(_protocol_line[3]);
  uint8_t value_08 = hex_to_int(_protocol_line[4]);
  uint8_t value_04 = hex_to_int(_protocol_line[5]);
  uint8_t value_00 = hex_to_int(_protocol_line[6]);
  if( value_12 < 0 || value_12 > 15 ||
      value_08 < 0 || value_08 > 15 || 
      value_04 < 0 || value_04 > 15 ||
      value_00 < 0 || value_00 > 15 )
  {
    protocol_output_error( "value", 5 ); 
    return;
  }
  
  uint16_t value = (((uint16_t)value_12) << 12) |
                  (((uint16_t)value_08) << 8) |
                  (((uint16_t)value_04) << 4) |
                  (((uint16_t)value_00) << 0);

  if( value < 10 || value > 10000 )
  {
    protocol_output_error( "limit", 5 ); 
    return;
  }
  
  set_blank_period( value );
    
  protocol_parse_blank_read();
}

void protocol_parse_blank_read()
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < 8 ) // "FR:0010\n"
   return;

  protocol_put_output_byte( UART_COMMAND_BLANK );
  protocol_put_output_byte( UART_MODE_READ );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  protocol_put_output_byte( _hex_table[(_flash_blank_period >> 12) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_blank_period >>  8) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_blank_period >>  4) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_blank_period >>  0) & 0x0F] );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_interval_write()
{
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t value_12 = hex_to_int(_protocol_line[3]);
  uint8_t value_08 = hex_to_int(_protocol_line[4]);
  uint8_t value_04 = hex_to_int(_protocol_line[5]);
  uint8_t value_00 = hex_to_int(_protocol_line[6]);
  if( value_12 < 0 || value_12 > 15 ||
      value_08 < 0 || value_08 > 15 || 
      value_04 < 0 || value_04 > 15 ||
      value_00 < 0 || value_00 > 15 )
  {
    protocol_output_error( "value", 5 ); 
    return;
  }
  
  uint16_t value = (((uint16_t)value_12) << 12) |
                  (((uint16_t)value_08) << 8) |
                  (((uint16_t)value_04) << 4) |
                  (((uint16_t)value_00) << 0);

  if( value < 10 || value > 10000 )
  {
    protocol_output_error( "limit", 5 ); 
    return;
  }
  
  set_interval_period( value );
    
  protocol_parse_interval_read();
}

void protocol_parse_interval_read()
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < 8 ) // "FR:0010\n"
   return;

  protocol_put_output_byte( UART_COMMAND_INTERVAL );
  protocol_put_output_byte( UART_MODE_READ );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  protocol_put_output_byte( _hex_table[(_flash_interval_period >> 12) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_interval_period >>  8) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_interval_period >>  4) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_flash_interval_period >>  0) & 0x0F] );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_sim_write()
{
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t value_hi = hex_to_int(_protocol_line[3]);
  uint8_t value_lo = hex_to_int(_protocol_line[4]);
  if( value_lo < 0 || value_lo > 15 ||
      value_hi < 0 || value_hi > 15 )
  {
    protocol_output_error( "value", 5 ); 
    return;
  }
  
  uint8_t value  = value_lo | (((uint16_t)value_hi) << 4);

  if( value < 50 )
  {
    protocol_output_error( "limit", 5 ); 
    return;
  }
  
  _simulation_period = value;
  
  set_interval_simulator(_simulation_period);
  
  protocol_parse_sim_read();
}

void protocol_parse_sim_read()
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  // if overflow
  if( space_availible < 6 ) // "SR:10\n"
    return;

  protocol_put_output_byte( UART_COMMAND_SIMULATION );
  protocol_put_output_byte( UART_MODE_READ );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  protocol_put_output_byte( _hex_table[(_simulation_period >>  4) & 0x0F] );
  protocol_put_output_byte( _hex_table[(_simulation_period >>  0) & 0x0F] );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void protocol_parse_pattern_write()
{
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER ||
      _protocol_line[4] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t index = hex_to_int(_protocol_line[3]);
  if( index < 0 || index > 15 )
  {
    protocol_output_error( "index", 9 ); 
    return;
  }
  
  for( uint8_t i = 0, j = 0; i < LED_LINE_LENGTH; i++ )
  {
     uint8_t value_hi = hex_to_int(_protocol_line[5 + j++]);
     uint8_t value_lo = hex_to_int(_protocol_line[5 + j++]);
     if( _protocol_line[5 + j++] != UART_CHARACTER_COMMA )
     {
        protocol_output_error( "comma", 5 ); 
        return;
     }

     if( value_lo < 0 || value_lo > 15 ||
         value_hi < 0 || value_hi > 15 )
     {
        protocol_output_error( "value", 5 ); 
        return;
     }
     
     pattern_array[index][i] = value_lo | (((uint16_t)value_hi) << 4);
  }
  
  line_array_init( index, pattern_array[index] );
  
  protocol_parse_pattern_read();
}

void protocol_parse_pattern_read()
{
  uint8_t space_availible = UART_MAX_WRITE_LENGTH - _write_buffer.count;
  
  if( _protocol_line[2] != UART_CHARACTER_DELIMITER )
  {
    protocol_output_error( "delimiter", 9 ); 
    return;
  }

  uint8_t index = hex_to_int(_protocol_line[3]);
  if( index < 0 || index > 15 )
  {
    protocol_output_error( "index", 5 ); 
    return;
  }
  
  // if overflow
  if( space_availible < 20 ) // "PR:1:00,01,02,03,04\n"
   return;

  protocol_put_output_byte( UART_COMMAND_PATTERN );
  protocol_put_output_byte( UART_MODE_READ );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  protocol_put_output_byte( _hex_table[index] );
  protocol_put_output_byte( UART_CHARACTER_DELIMITER );
  for( uint8_t i = 0; i < LED_LINE_LENGTH; i++ )
  {
    protocol_put_output_byte( _hex_table[(pattern_array[index][i] >>  4) & 0x0F] );
    protocol_put_output_byte( _hex_table[(pattern_array[index][i] >>  0) & 0x0F] );
    protocol_put_output_byte( UART_CHARACTER_COMMA );
  }
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
 void protocol_help()
{
  protocol_output_string( "HW: FR/FW-flash period", 22 );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );

  protocol_output_string( "HW: BR/BW-blank period", 22 );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );

  protocol_output_string( "HW: IR/IW-interval period", 25 );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );

  protocol_output_string( "HW: SR/SW-simulation period", 27 );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );

  protocol_output_string( "HW: PR/PW-pattern", 17 );
  protocol_put_output_byte( UART_CHARACTER_EOL );
  protocol_put_output_byte( UART_CHARACTER_NEWLINE );
}

