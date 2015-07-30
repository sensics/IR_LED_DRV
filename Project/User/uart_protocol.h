#ifndef __UART_PROTOCOL__
#define __UART_PROTOCOL__

uint8_t protocol_is_output_ready();
uint8_t protocol_get_output_byte();
void protocol_put_input_byte( uint8_t ch );

#endif