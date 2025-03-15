#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "test_w204.h"
#include "suite.h"
#include "w204.h"
#include "spi.h"
#include "uart.h"
#include "led_lib.h"
#include "switches.h"

/* Define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 10000000UL
#endif

static stream_out_t stream_out;

void test_w204(void) {
    
    cli(); 
         
    led_init();
    uart_init();
    switch_init();
    spi_init( &spi_config );  
     
    sei();
    
    w204_init( SPI_TEST_PORT );
    w204_set_stream_out( &stream_out );
    
    w204_move_cursor(LINE1, 0);
    w204_puts("Hello AVR");
    
    /* Fill Test Stream With Data */
    sprintf(stream_out.data0, "SENSOR00");
    sprintf(stream_out.data1, "SENSOR01");
    sprintf(stream_out.data2, "SENSOR02");
    sprintf(stream_out.data3, "SENSOR03");
    sprintf(stream_out.data4, "SENSOR04");
    sprintf(stream_out.data5, "SENSOR05");
}