/*************************************************************************
* Title     : w204.c
* Author    : Dimitri Dening
* Created   : 11.04.2023
* Software  : Microchip Studio V7
* Hardware  : Atmega2560, EA W204-XLG
        
DESCRIPTION:
    EA W204-XLG Display Driver using the SPI Procotol.
USAGE:
    see <w204.h>
NOTES:
                       
*************************************************************************/

/* General libraries */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* User defined libraries */
#include "w204.h"
#include "spi.h"

static void _w204_send_fake_10_bit_instruction( uint8_t, uint8_t );
static void _w204_send_fake_10_bit_instructions( uint8_t, uint8_t, uint8_t, uint8_t );
static void _w204_send_fake_10_bit_data( uint8_t, uint8_t, uint8_t );
static void _w204_send_fake_10_bit_data_stream( uint8_t, uint8_t, char* );
static void _w204_read_fake_10_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_read_8_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_check_busy ( void );
static void _w204_check_fake_busy ( void );
static void _w204_putc( uint8_t c );
static void _w204_hello_word( void );

volatile uint8_t READING;
void callback_read_finished( void ) {
    READING = 0;
}

static device_t* spi_device;
static stream_out_t* stream_out = NULL;
static uint8_t CURRENT_STREAM_LINE = 0x00;

void w204_init( uint8_t cs ) {
       
    spi_device = spi_create_device( cs, cs, cs );
     
    w204_send_8_bit_instruction( RSRW00, FUNCTION_SET_EUROPEAN ); // has to be sent first!
    w204_send_8_bit_instruction( RSRW00, DISPLAY_OFF );
    w204_send_8_bit_instruction( RSRW00, CURSOR_DIR_LEFT_NO_SHIFT );
    w204_send_8_bit_instruction( RSRW00, CHARACTER_MODE_INTERNAL_PWR );
    w204_send_8_bit_instruction( RSRW00, CLEAR_DISPLAY ); 
    w204_send_8_bit_instruction( RSRW00, RETURN_HOME );
    w204_send_8_bit_instruction( RSRW00, DISPLAY_ON | CURSOR_ON | BLINK_ON );  
}

// TODO: Analyze real response signal from device. Use check_fake_busy() meanwhile.
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

static void _w204_send_fake_10_bit_data( uint8_t opcode, uint8_t data1, uint8_t data2 ) {
    
    spi_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * 3 );
    
    if ( data == NULL ) {
        return;
    }
    
    data[0] = ( opcode << 6 ) | ( data1 >> 2 );
    data[1] = ( data1 << 6 ) | ( data2 >> 2 );
    data[2] = ( data2 << 6 );
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, data, 3, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
    
    free( data );
}

static void _w204_send_fake_10_bit_data_stream( uint8_t opcode, uint8_t stream_len, char* stream) {
        
    spi_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * stream_len );
    
    if ( data == NULL ) {
        return;
    }
    
    data[0] = ( opcode << 6 ) | ( stream[0] >> 2 );
    
    for ( uint8_t i = 0; i < stream_len - 1; i++ ) {
        data[i + 1] = ( stream[i] << 6 ) | ( stream[i+1] >> 2 );
    }
    
    // Select stream_len - 2, because we artifically increased the length before by 1.
    // If we only reduce the value by 1 here, we will access the value '\0' and shift nothing.
    data[stream_len - 1] = ( stream[stream_len - 2] << 6 );
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, data, stream_len, NULL );
    
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

/* Creates a spi job for each character in the string */
void w204_puts( char* string ) {
    while( *string ) {
        _w204_putc( *string++ );
    }
}

/* Creates a single spi job for the entire string stream */
void w204_put_stream( char* stream ) {
    _w204_check_fake_busy();
    _w204_send_fake_10_bit_data_stream( RSRW10, strlen(stream), stream );
}

void w204_move_cursor( uint8_t line, uint8_t offset ) {
    w204_send_8_bit_instruction( RSRW00, DDRAM_ADDR + line + offset );
}

void w204_shift_cursor_left( void ) {   
    w204_send_8_bit_instruction( RSRW00, SHIFT_INSTRUCTION | SHIFT_CURSOR_LEFT );
}

void w204_shift_cursor_right( void ) {
    w204_send_8_bit_instruction( RSRW00, SHIFT_INSTRUCTION | SHIFT_CURSOR_RIGHT );
}

void w204_shift_display_left( void ) {
    w204_send_8_bit_instruction( RSRW00, SHIFT_INSTRUCTION | SHIFT_DISPLAY_LEFT );
}

void w204_shift_display_right( void ) {
    w204_send_8_bit_instruction( RSRW00, SHIFT_INSTRUCTION | SHIFT_DISPLAY_RIGHT);
}

void w204_shift_display_up( void ) {
    if ( CURRENT_STREAM_LINE != 0 ) {
        CURRENT_STREAM_LINE--;
    }
       
    w204_update( stream_out );
}

void w204_shift_display_down( void ) {
    if ( CURRENT_STREAM_LINE != (MAX_OUTPUT_STREAMS - LINE_COUNT ) ) {
        CURRENT_STREAM_LINE++;
    }
    
    w204_update( stream_out );
}

void w204_clear( void ) {
    w204_send_8_bit_instruction( RSRW00, CLEAR_DISPLAY );
}

void w204_update( stream_out_t* stream ) {
        
    if ( stream == NULL ) {
        return;
    }
    
    if ( CURRENT_STREAM_LINE > MAX_OUTPUT_STREAMS - LINE_COUNT || 
         CURRENT_STREAM_LINE < 0 ) {
        return;
    }
    
    char* _stream[MAX_OUTPUT_STREAMS] = {
        stream->data0,
        stream->data1,
        stream->data2,
        stream->data3,
        stream->data4,
        stream->data5
    };
    
    uint8_t _stream_line[LINE_COUNT] = { LINE1, LINE2, LINE3, LINE4 };
    
    uint8_t _CURRENT_STREAM_LINE_ = CURRENT_STREAM_LINE;
    
    // w204_clear(); // -> Doesn't work with w204_put_stream yet. Produces wrong line output instead of resetting to addr. 0
     
    for (uint8_t line = 0; line < LINE_COUNT; line++) {
        w204_move_cursor( _stream_line[line], 0 );
        w204_put_stream( _stream[_CURRENT_STREAM_LINE_] );
        _CURRENT_STREAM_LINE_++;
    }
}

void w204_set_stream_out( stream_out_t* stream ) {
    stream_out = stream;
}