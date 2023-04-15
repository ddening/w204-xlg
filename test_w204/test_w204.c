#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
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

int main( void ) {
    
    cli(); 
         
    led_init();
    uart_init();
    switch_init();
    spi_init( &spi_config );  
     
    sei();
    
    w204_init( SPI_TEST_PORT );
   
   while (1) { /* Busy-wait forever. */ }
}