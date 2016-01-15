#ifndef MAX7221spi_H
#define MAX7221spi_H

#include "Arduino.h"

// SHUTDOWN options
#define ON 0x0 // shutdown active, screen blank
#define OFF 0x1 // shutdown not active

// DISPLAY_TEST options
#define NORMAL 0x0
#define TEST 0x1

class MAX7221spi 
{  
	private:
	void transmit(unsigned char address, unsigned char data);
	unsigned char no_of_digits;

  public:
  void initialize(unsigned char no_Of_digits);
	void setBrightness(unsigned char brightness);
	void shutdown(unsigned char choice);
	void testDisplay(unsigned char choice);
	void print(float fnum, unsigned char precision);
	
  // empty constructor
  MAX7221spi(){}

};

#endif
