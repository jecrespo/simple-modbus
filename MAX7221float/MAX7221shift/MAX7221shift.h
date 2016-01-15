#ifndef MAX7221shift_H
#define MAX7221shift_H

#include "Arduino.h"

// SHUTDOWN options
#define ON 0x0 // shutdown active, screen blank
#define OFF 0x1 // shutdown not active

// DISPLAY_TEST options
#define NORMAL 0x0
#define TEST 0x1

class MAX7221shift 
{  
	private:
	void transmit(byte address, byte data);
	unsigned char clockPin, loadPin, dataPin;
	unsigned char no_of_digits;

  public:
  void initialize(unsigned char _clockPin, unsigned char _loadPin, unsigned char _dataPin, unsigned char _no_of_digits);
	void setBrightness(unsigned char brightness);
	void shutdown(unsigned char choice);
	void testDisplay(unsigned char choice);
	void print(float fnum, unsigned char precision);
	
  // empty constructor
  MAX7221shift(){}

};

#endif
