#ifndef TEST_W204_H_
#define TEST_W204_H_

#include <avr/io.h>

#if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega16__)
#   define SPI_TEST_PORT PORTB4
# elif defined(__AVR_ATmega2560__)
#   define SPI_TEST_PORT PB0
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "NO SPI_TEST_PORT DEFINED FOR MICROCONTROLLER"
#  endif
#endif

#endif /* TEST_W204_H_ */