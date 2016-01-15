#ifndef MULTIPLEX7SEG_H
#define MULTIPLEX7SEG_H

#include "Arduino.h"
#include <avr/interrupt.h>

namespace Multiplex7Seg 
{
	extern unsigned long msecs;
	extern volatile unsigned long count;
	extern volatile char overflowing;
	extern volatile unsigned int tcnt2;
	extern byte digits[4];
	extern byte noOfDigits;
	extern byte enableZB;
	extern byte *digitsPtr; 
	extern byte *digitPinsPtr;
	extern byte *segmentPinsPtr; 
	extern byte digitValueCurrent;
  extern byte digitValuePrevious; 
	void set(byte _enableZB, byte _noOfDigits, byte *digitPins, byte *segmentPins);
	void _overflow();
	void loadValue(int value);
	void update();
}
#endif