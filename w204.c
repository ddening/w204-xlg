#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#include "w204.h"
#include "spi.h"
#include "uart.h"

static device_t* spi_device;

static void _w204_send_fake_10_bit( uint8_t, uint8_t );
static void _w204_read_fake_10_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_read_8_bit( uint8_t, uint8_t, uint8_t* );
static void _w204_check_busy ( void );
static void _w204_check_fake_busy ( void );
static void _w204_hello_word( void );

// Callback Functions
volatile uint8_t READING;
void callback_read_finished( void ) {
    READING = 0;
}

static uint8_t character_h = 0x48 ; // 0b01001000
static uint8_t character_e = 0x65 ; // 0b01100101
static uint8_t character_l = 0x6C ; // 0b01101100
static uint8_t character_o = 0x6F ; // 0b01101111

void w204_init( uint8_t cs ) {
    
    cli();
    
    spi_init( &spi_config );
    
    sei();
    
    spi_device = spi_create_device( cs, cs, cs );
     
    w204_send_8_bit( RSRW00, FUNCTION_SET_EUROPEAN ); // has to be sent first!
    w204_send_8_bit( RSRW00, DISPLAY_OFF );
    w204_send_8_bit( RSRW00, CURSOR_DIR_LEFT_NO_SHIFT );
    w204_send_8_bit( RSRW00, CHARACTER_MODE_INTERNAL_PWR );
    w204_send_8_bit( RSRW00, CLEAR_DISPLAY ); 
    w204_send_8_bit( RSRW00, RETURN_HOME );
    w204_send_8_bit( RSRW00, DISPLAY_ON | CURSOR_ON | BLINK_ON );
    
    _w204_hello_word();
}

static void _w204_hello_word( void ) {
    
    // SET DDRAM/CGRAM ADDRESS := RSRW00
    // WRITE DATA INTO DDRAM   := RSRW10 
    w204_send_8_bit( RSRW10, character_h );
    w204_send_8_bit( RSRW10, character_e );
    w204_send_8_bit( RSRW10, character_l );
    w204_send_8_bit( RSRW10, character_l );
    w204_send_8_bit( RSRW10, character_o );
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
    
        _w204_read_8_bit( RSRW01, 0x00, container );
    
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
    
    uint8_t* _data_write = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    uint8_t* _data_read  = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( _data_write == NULL || _data_read == NULL ) {
        return;
    }
    
    _data_write[0] = ( opcode | ( instruction >> 2 ) );
    _data_write[1] = ( instruction << 6 );
    
    _data_read[0] = 0x00;
    _data_read[1] = 0x00;
    
    payload_t* payload1 = payload_create_spi( PRIORITY_NORMAL, spi_device, _data_write, 2, NULL );
    payload_t* payload2 = payload_create_spi( PRIORITY_NORMAL, spi_device, _data_read , 2, callback_read_finished );
    
    if ( payload1 == NULL || payload2 == NULL ) {
        return;
    }
    
    err = spi_read_write( payload1, payload2, container );
}

void _w204_read_8_bit( uint8_t opcode, uint8_t instruction, uint8_t* container ) {
    _w204_read_fake_10_bit( opcode, instruction, container ); 
}

static void _w204_send_fake_10_bit( uint8_t opcode, uint8_t instruction ) {
    
    spi_error_t err;
    
    uint8_t* _data = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( _data == NULL ) {
        return;
    }
   
    _data[0] = ( opcode | ( instruction >> 2 ) );
    _data[1] = ( instruction << 6 );
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, _data, 2, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
}

void w204_send_8_bit( uint8_t opcode, uint8_t instruction ) {    
   _w204_check_busy();
   _w204_send_fake_10_bit( opcode, instruction );
}