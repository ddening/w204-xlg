#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "w204.h"
#include "spi.h"
#include "uart.h"

static device_t* spi_device;

static void _w204_send_fake_10_bit_instruction( uint8_t, uint8_t );
static void _w204_send_fake_10_bit_instructions( uint8_t, uint8_t, uint8_t, uint8_t );
static void _w204_send_fake_10_bit_data( uint8_t, uint8_t, uint8_t );
static void _w204_read_fake_10_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_read_8_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_check_busy ( void );
static void _w204_check_fake_busy ( void );
static void _w204_putc( uint8_t c );
static void _w204_hello_word( void );

// Callback Functions
volatile uint8_t READING;
void callback_read_finished( void ) {
    READING = 0;
}

static uint8_t char_h = 0x48 ; // 0b01001000
static uint8_t char_e = 0x65 ; // 0b01100101
static uint8_t char_l = 0x6C ; // 0b01101100
static uint8_t char_o = 0x6F ; // 0b01101111

void w204_init( uint8_t cs ) {
    
    cli();
    
    spi_init( &spi_config );
    
    sei();
    
    spi_device = spi_create_device( cs, cs, cs );
     
    w204_send_8_bit_instruction( RSRW00, FUNCTION_SET_EUROPEAN ); // has to be sent first!
    w204_send_8_bit_instruction( RSRW00, DISPLAY_OFF );
    w204_send_8_bit_instruction( RSRW00, CURSOR_DIR_LEFT_NO_SHIFT );
    w204_send_8_bit_instruction( RSRW00, CHARACTER_MODE_INTERNAL_PWR );
    w204_send_8_bit_instruction( RSRW00, CLEAR_DISPLAY ); 
    w204_send_8_bit_instruction( RSRW00, RETURN_HOME );
    w204_send_8_bit_instruction( RSRW00, DISPLAY_ON | CURSOR_ON | BLINK_ON );
      
    // _w204_hello_word();
    w204_puts("Hello World");
}

static void _w204_hello_word( void ) {   
    w204_send_8_bit_data( RSRW10, char_h );
    w204_send_8_bit_data( RSRW10, char_e );
    w204_send_8_bit_data( RSRW10, char_l );
    w204_send_8_bit_data( RSRW10, char_l );
    w204_send_8_bit_data( RSRW10, char_o );
}
/* Check BUSY FLAG (BF) RESPONSE  
If the response equals 1 the process is still active and we have to do another read request.      
Response format (10 bit): 0 1 | BF AC AC AC AC AC AC AC
 -> First 8 bit looks like this: 0 1 BF AC AC AC AC AC
 -> We have to check the third bit from the left
*/
static void _w204_check_busy ( void ) {
    
    uint8_t BUSY = 1;
    
    uint8_t* container = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( container == NULL ) {
        return;
    }
    
    while ( BUSY ) {
        
        READING = 1;
    
        _w204_read_8_bit( RSRW01, 0x80, container );
    
        while ( READING );
           
        uart_put( "%i, %i", container[0], container[1] );
        
        if ( container[0] & ( 1 << 5 ) ) {
            BUSY = 1;
        } else {
            BUSY = 0;
        }
    }
    
    free( container );
}

static void _w204_check_fake_busy ( void ) {
    _delay_ms(50); // Fake Busy Response
}

static void _w204_read_fake_10_bit( uint8_t opcode, uint8_t instruction, uint8_t* container ) {
    
    spi_error_t err;
    
    uint8_t* data_write = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    uint8_t* data_read  = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( data_write == NULL || data_read == NULL ) {
        return;
    }
    
    data_write[0] = ( ( opcode << 6 ) | ( instruction >> 2 ) );
    data_write[1] = ( instruction << 6 );
    
    data_read[0] = 0x00;
    data_read[1] = 0x00;
    
    payload_t* payload1 = payload_create_spi( PRIORITY_NORMAL, spi_device, data_write, 2, NULL );
    payload_t* payload2 = payload_create_spi( PRIORITY_NORMAL, spi_device, data_read , 2, &callback_read_finished );
    
    if ( payload1 == NULL || payload2 == NULL ) {
        return;
    }
    
    err = spi_read_write( payload1, payload2, container );
    
    free(data_write);
    free(data_read);
}

static void _w204_read_8_bit( uint8_t opcode, uint8_t instruction, uint8_t* container ) {
    _w204_read_fake_10_bit( opcode, instruction, container );
}

static void _w204_send_fake_10_bit_instruction( uint8_t opcode, uint8_t instruction ) {
    
    spi_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( data == NULL ) {
        return;
    }
        
    data[0] = ( ( opcode << 6) | ( instruction >> 2 ) );
    data[1] = ( instruction << 6 );
        
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, data, 2, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
    
    free( data );
}

static void _w204_send_fake_10_bit_instructions( uint8_t opcode1, uint8_t instruction1, uint8_t opcode2, uint8_t instruction2 ) {
    
    spi_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * 3 );
    
    if ( data == NULL ) {
        return;
    }
       
    data[0] = ( opcode1 << 6 ) | ( instruction1 >> 2 );
    data[1] = ( instruction1 << 6 ) | ( opcode2 << 4 ) | ( instruction2 >> 4 );
    data[2] = ( instruction2 << 4 );
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, data, 3, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
    
    free( data );
}

static void _w204_send_fake_10_bit_data( uint8_t opcode1, uint8_t data1, uint8_t data2 ) {
    
    spi_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * 3 );
    
    if ( data == NULL ) {
        return;
    }
    
    data[0] = ( opcode1 << 6 ) | ( data1 >> 2 );
    data[1] = ( data1 << 6 ) | ( data2 >> 2 );
    data[2] = ( data2 << 6 );
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, data, 3, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
    
    free( data );
}

void w204_send_8_bit_instruction( uint8_t opcode, uint8_t instruction ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_instruction( opcode, instruction );
}

void w204_send_8_bit_instructions( uint8_t opcode1, uint8_t instruction1, uint8_t opcode2, uint8_t instruction2 ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_instructions( opcode1, instruction1, opcode2, instruction2 );
}

void w204_send_8_bit_data( uint8_t opcode, uint8_t data ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_instruction( opcode, data ); /* We can reuse the single instruction function. It has the same format as the data transmission layout */
}

void w204_send_8_bit_data_n( uint8_t opcode, uint8_t data1, uint8_t data2 ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_data( opcode, data1, data2 );
}

static void _w204_putc( uint8_t c ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_instruction( RSRW10, c );
}

void w204_puts( char* string ) {
    while( *string ) {
        _w204_putc( *string++ );
    }
}

void w204_move_cursor( uint8_t line, uint8_t offset ) {
    w204_send_8_bit_instruction( RSRW00, DDRAM_ADDR + line + offset );
}