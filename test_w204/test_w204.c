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

static device_t* spi_device;

void main( void ) {
    
    	cli();
    	
    	led_init();
    	
    	uart_init();
    	
    	spi_init(&spi_config);
    	
    	sei();
    	
    	spi_device = spi_create_device(SPI_TEST_PORT, SPI_TEST_PORT, SPI_TEST_PORT);
    	
    	//DEFINE_TEST_CASE(data_flash_read_test, NULL, run_spi_flash_read_test, NULL, "SPI data flash read test");
    	//DEFINE_TEST_CASE(data_transfer_test, NULL, run_spi_transfer_test, NULL, "SPI data transfer test");
    	//DEFINE_TEST_CASE(memory_leak_test, NULL, run_spi_memory_leak_test, NULL, "SPI memory leak test");
//
    	///* Put test case addresses in an array */
    	//DEFINE_TEST_ARRAY(spi_tests) = {
        	//&data_flash_read_test,
        	//&data_transfer_test,
        	//&memory_leak_test
    	//};
    	//
    	///* Define the test suite */
    	//DEFINE_TEST_SUITE(spi_suite, spi_tests, "SPI driver test suite");
    	//
    	///* Run all tests in the test suite */
    	//test_suite_run(&spi_suite);
    	
    	while (1) { /* Busy-wait forever. */ }
}