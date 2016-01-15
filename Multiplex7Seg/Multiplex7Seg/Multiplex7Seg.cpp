/*
	Multiplex7Seg is a library that allows you to multiplex 1, 2 ,3 or 4 
	7 segment digits using any one of the pins on your arduino. 
	More than 4 digits is not practical since the brightness decreases with
	a faster refresh rate and the MsTimer2 library only goes to 1ms.
	This is enough for 4 digits with good illumination.
	At the moment the digits can only display positive values from 0000 - 9999.
	The refresh rate interrupt is set at 1ms in practice this is not reach since
	there is some time needed to write the segment pins high or low.
	The interrupt routine code used is from the MsTimer2 libary.
	The library was created on 11/Jan/2011 using the MsTimer2 library
	at that time. See below.
	
  MsTimer2.h - Using timer2 with 1ms resolution
  Javier Valencia <javiervalencia80@gmail.com>
  
  History:
  	29/May/09 - V0.5 added support for Atmega1280 (thanks to Manuel Negri)
  	19/Mar/09 - V0.4 added support for ATmega328P (thanks to Jerome Despatis)
  	11/Jun/08 - V0.3 
  		changes to allow working with different CPU frequencies
  		added support for ATMega128 (using timer2)
  		compatible with ATMega48/88/168/8
	10/May/08 - V0.2 added some security tests and volatile keywords
	9/May/08 - V0.1 released working on ATMEGA168 only
	

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "Multiplex7Seg.h"

unsigned long Multiplex7Seg::msecs;
volatile unsigned long Multiplex7Seg::count;
volatile char Multiplex7Seg::overflowing;
volatile unsigned int Multiplex7Seg::tcnt2;
byte Multiplex7Seg::digits[4];
byte Multiplex7Seg::noOfDigits;
byte Multiplex7Seg::enableZB;
byte *Multiplex7Seg::digitsPtr; 
byte *Multiplex7Seg::digitPinsPtr;
byte *Multiplex7Seg::segmentPinsPtr; 
byte Multiplex7Seg::digitValueCurrent;
byte Multiplex7Seg::digitValuePrevious; 

void Multiplex7Seg::set(byte _enableZB, byte _noOfDigits, byte *digitPins, byte *segmentPins) // digitPins - LSB to MSB, segmentPins - a to g
{
	float prescaler = 0.0;
	
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2B |= ((1<<CS22) | (1<<CS20));
		TCCR2B &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	ASSR &= ~(1<<AS2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2 |= ((1<<CS22) && (1<<CS20));
		TCCR2 &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		TCCR2 &= ~(1<<CS22);
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 256
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 256.0;
	}
#endif

	tcnt2 = 256 - (int)((float)F_CPU * 0.001 / prescaler);
	
	count = 0;
	overflowing = 0;
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TCNT2 = tcnt2;
	TIMSK2 |= (1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TCNT2 = tcnt2;
	TIMSK |= (1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TCNT2 = tcnt2;
	TIMSK |= (1<<TOIE2);
#endif

	msecs = 3;
	byte i;
  for (i = 0; i < _noOfDigits; i++) // initialize digit pins as OUTPUTS
    pinMode(digitPins[i], OUTPUT);
  for (i = 0; i < 7; i++) // initialize segment pins as OUTPUTS
    pinMode(segmentPins[i], OUTPUT); 
	enableZB = _enableZB; // set zero leading blanking - 0 == OFF,  1 == ON
	noOfDigits = _noOfDigits; 
	digitsPtr = digits; // initialize digits pointer
  digitPinsPtr = digitPins; // initialize digit pins pointer
  segmentPinsPtr = segmentPins; // initialize segment pins pointer
	digitValueCurrent = 0;
	digitValuePrevious = 0;	
}

void Multiplex7Seg::_overflow() 
{
	count += 1;
	if (count >= msecs && !overflowing) 
	{
		overflowing = 1;
		count = 0;
		update();
		overflowing = 0;
	}
}

void Multiplex7Seg::loadValue(int value) // extract the integer value into decimal values
{
  *(digitsPtr + 3) = value / 1000;
  value %= 1000;
  *(digitsPtr + 2) = value / 100;
  value %= 100;
  *(digitsPtr + 1) = value / 10;
  *digitsPtr = value % 10;
}

void Multiplex7Seg::update() // interrupt activates update()
{
    digitalWrite(*(digitPinsPtr + digitValuePrevious), LOW);
    switch (*(digitsPtr + digitValueCurrent))
    {
      case 0: // B00111111 
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), HIGH);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), LOW);
      break;
      case 1:  // B00000110
      digitalWrite(*segmentPinsPtr, LOW);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), LOW);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), LOW);
      digitalWrite(*(segmentPinsPtr + 6), LOW);
      break;
      case 2: // B01011011
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), LOW);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), HIGH);
      digitalWrite(*(segmentPinsPtr + 5), LOW);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 3:   // B01001111 
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), LOW);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 4: // B01100110
      digitalWrite(*segmentPinsPtr, LOW);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), LOW);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 5: // B01101101
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), LOW);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 6: // B01111100
      digitalWrite(*segmentPinsPtr, LOW);
      digitalWrite(*(segmentPinsPtr + 1), LOW);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), HIGH);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 7: // B00000111
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), LOW);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), LOW);
      digitalWrite(*(segmentPinsPtr + 6), LOW);
      break;
      case 8: // B01111111
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), HIGH);
      digitalWrite(*(segmentPinsPtr + 4), HIGH);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
      case 9: // B01100111
      digitalWrite(*segmentPinsPtr, HIGH);
      digitalWrite(*(segmentPinsPtr + 1), HIGH);
      digitalWrite(*(segmentPinsPtr + 2), HIGH);
      digitalWrite(*(segmentPinsPtr + 3), LOW);
      digitalWrite(*(segmentPinsPtr + 4), LOW);
      digitalWrite(*(segmentPinsPtr + 5), HIGH);
      digitalWrite(*(segmentPinsPtr + 6), HIGH);
      break;
    }
		if (enableZB)
		{
			if (digitValueCurrent == 0)
				digitalWrite(*digitPinsPtr, HIGH);
			else if (digitValueCurrent == 1)
			{
				if (!((*(digitsPtr + 3) == 0) && (*(digitsPtr + 2) == 0) && (*(digitsPtr + 1) == 0)))
				digitalWrite(*(digitPinsPtr + 1), HIGH);
			}
			else if (digitValueCurrent == 2)
			{
				if (!((*(digitsPtr + 3) == 0) && (*(digitsPtr + 2) == 0)))
					digitalWrite(*(digitPinsPtr + 2), HIGH);
			}
			else if (digitValueCurrent == 3)
			{
				if (*(digitsPtr + 3) != 0)
					digitalWrite(*(digitPinsPtr + 3), HIGH);
			}
		}
		else
			digitalWrite(*(digitPinsPtr + digitValueCurrent), HIGH);
		digitValuePrevious = digitValueCurrent;
		digitValueCurrent++;
		if (digitValueCurrent == noOfDigits)
			digitValueCurrent = 0;
}

ISR(TIMER2_OVF_vect) 
{
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TCNT2 = Multiplex7Seg::tcnt2;
#elif defined (__AVR_ATmega128__)
	TCNT2 = Multiplex7Seg::tcnt2;
#elif defined (__AVR_ATmega8__)
	TCNT2 = Multiplex7Seg::tcnt2;
#endif
	Multiplex7Seg::_overflow();
}

