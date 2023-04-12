#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
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

// Callback functions
volatile uint8_t READING;
void callback_read_finished( void ) {
    READING = 0;
}

/* 
   We have to create fake packets to create the desired 10 bit format, since we can only
   8 bit with each packet.
   
   Design concept:
   - Packet D00 and D01 contain only 0's as a pre-stream of bits.
   - D02 and D03 contain the 2 bit opcode + 8 bit data
   - D02 contains the 2 bit opcode in the first two bits
   - D03 contains the instruction and doesn't need to be modified
   - Final dataword/payload always has the length of 4
   
   .xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.
   .00000000.00000000.00000000.00000000.00000000.00000000.00000000.
   .  D00   .  D01   .  D02   .  D03   .  D04   .	D05 .   D06 

*/

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
}

/* 
Check BUSY FLAG (BF) RESPONSE  
If the response equals 1 the process is still active and
we have to do another read request.
       
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
    
        _w204_read_8_bit( RSRW01, READ_BUSY_FLAG, container );
    
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
    
    uint8_t* _data_write = (uint8_t*) malloc( sizeof( uint8_t ) * 4 );
    uint8_t* _data_read  = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( _data_write == NULL || _data_read == NULL ) {
        return;
    }
    
    _data_write[0] = 0x00;
    _data_write[1] = 0x00;
    _data_write[2] = opcode;
    _data_write[3] = instruction;
    
    _data_read[0] = 0x00;
    _data_read[1] = 0x00;
    
    payload_t* payload1 = payload_create_spi( PRIORITY_NORMAL, spi_device, _data_write, 4, NULL );
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
    
    uint8_t* _data = (uint8_t*) malloc( sizeof( uint8_t ) * 4 );
    
    if ( _data == NULL ) {
        return;
    }
    
    _data[0] = 0x00;
    _data[1] = 0x00;
    _data[2] = opcode;
    _data[3] = instruction;
    
    payload_t* payload = payload_create_spi( PRIORITY_NORMAL, spi_device, _data, 4, NULL );
    
    if ( payload == NULL ) {
        return;
    }
    
    err = spi_write( payload );
}

void w204_send_8_bit( uint8_t opcode, uint8_t instruction ) {    
   _w204_check_busy();
   _w204_send_fake_10_bit( opcode, instruction );
}