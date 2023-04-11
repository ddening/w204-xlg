#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "w204.h"
#include "spi.h"

static device_t* spi_device;

static void     _w204_send_fake_10_bit( uint8_t );
static uint8_t* _w204_read_fake_10_bit( uint8_t, uint8_t* );
static void     _w204_check_busy ( void );
static void     _w204_check_fake_busy ( void );

/* 
   We have to create fake packets to create the desired 10 bit format, since we can only
   8 bit with each packet.
   
   Design concept:
   - Packet D00 and D01 contain only 0's as a pre-stream of bits.
   - D02 and D03 contain the 2 bit opcode + 8 bit data
   - D02 contains the 2 bit opcode in the first two bits
   - D03 contains the instruction and doesn't need to be modified
   
   .xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.xxxxxxxx.xx.
   .00000000.00000000.00000000.00000000.00000000.00000000.00000000.
   .  D00   .  D01   .  D02   .  D03   .  D04   .	D05 .   D06 

*/

void w204_init( uint8_t cs ) {
    
    cli();
    
    spi_init( &spi_config );
    
    sei();
    
    spi_device = spi_create_device( cs, cs, cs );
     
    w204_send_8_bit( FUNCTION_SET_EUROPEAN ); // has to be sent first!
    w204_send_8_bit( DISPLAY_OFF );
    w204_send_8_bit( CURSOR_DIR_LEFT_NO_SHIFT );
    w204_send_8_bit( CHARACTER_MODE_INTERNAL_PWR );
    w204_send_8_bit( CLEAR_DISPLAY ); 
    w204_send_8_bit( RETURN_HOME );
    w204_send_8_bit( DISPLAY_ON | CURSOR_ON | BLINK_ON );
}

static void _w204_check_busy ( void ) {
    
    uint8_t* container = (uint8_t*) malloc( sizeof( uint8_t ) );
    
    if ( container == NULL ) {
        return;
    }
    
    w204_read_8_bit( READ_BUSY_FLAG, container );
}

static void _w204_check_fake_busy ( void ) {
    _delay_ms(5); // Fake Busy Response
}

static void _w204_send_fake_10_bit( uint8_t payload ) {
    
}

static uint8_t* _w204_read_fake_10_bit( uint8_t payload, uint8_t* container ) {
    return NULL;
}

void w204_send_8_bit( uint8_t payload ) {    
   _w204_check_fake_busy();
   _w204_send_fake_10_bit( payload );
}

uint8_t* w204_read_8_bit( uint8_t payload, uint8_t* container) {
    _w204_check_fake_busy();
    _w204_read_fake_10_bit( payload, container );
    
    return NULL;
}
