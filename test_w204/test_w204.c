#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "test_w204.h"
#include "suite.h"
#include "spi.h"
#include "uart.h"
#include "led_lib.h"

/* Define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 10000000UL
#endif

void main( void ) {
       	
    	led_init();
    	
    	uart_init();
    	
        uart_put( "Start W204 Suite" );
        
        w204_init( SPI_TEST_PORT );
          	
    	while (1) { /* Busy-wait forever. */ }
}