 /* 		
 MAX7221shift library that prints any float value of 8 digits
 on multiplex 7 segment displays.
 
 Postive float numbers have a range of  99999999.
 Negative float numbers have a range of -9999999.
 
 Using only one MAX7221 and is usually sufficient
 for displaying anything you want.
 
 In each case the decimal point will vary according to the
 precision value passed to print().
 
 MAX7221shift does not use the SPI library and thus you
 can use any pins for clock, load and data. This will
 be compatible with the MAX7219 also.
 
 You can also create more than one instance off a
 MAX7221shift class. 
 
 By Juan Bester : bester.juan@gmail.com
 
 */

#include "MAX7221shift.h"


// Address codes
#define NOP 0x0 // no operation
#define DECODE_MODE 0x9
#define INTENSITY 0xA // data (duty cycle) can be any value between 1 and 15
#define SCANLIMIT 0xB
#define SHUTDOWN 0xC
#define DISPLAY_TEST 0xF

// Digit special data codes
#define NEGATIVE_DIGIT 0xA
#define BLANK_DIGIT 0xF 

// DECODE_MODE options
#define NO_DECODE 0x0 
#define BCD_DECODE 0xFF

void MAX7221shift::initialize(unsigned char _clockPin, unsigned char _loadPin, unsigned char _dataPin, unsigned char _no_of_digits)
{
	// Max 8 digits and minimum 4 digits to be on the safe side
	if (_no_of_digits > 3 && _no_of_digits < 9)
		no_of_digits = _no_of_digits;
	else
		no_of_digits = 4;
	
	clockPin = _clockPin;
	loadPin = _loadPin;
	dataPin = _dataPin;
	
	pinMode(clockPin, OUTPUT);
	pinMode(loadPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	
	digitalWrite(loadPin, HIGH); // set load LOW for the MAX7219/7221 to receive
	
	// initialize the MAX7219/7221
	shutdown(OFF); // deactivate shutdown
  testDisplay(NORMAL); // deactivate testing of the display
	transmit(SCANLIMIT, no_of_digits - 1); // no_of_digits - 1 gives the address in hex
  transmit(DECODE_MODE, BCD_DECODE); // use BCD code
  setBrightness(15); // set to max brightness on startup
}

void MAX7221shift::setBrightness(unsigned char brightness)
{
	if (brightness < 1)
		brightness = 1;
	if (brightness > 15)
		brightness = 15;
	transmit(INTENSITY, brightness);
}

void MAX7221shift::shutdown(unsigned char choice)
{
	transmit(SHUTDOWN, choice);
}

void MAX7221shift::testDisplay(unsigned char choice)
{
	transmit(DISPLAY_TEST, choice);
}

void MAX7221shift::print(float fnum, unsigned char precision) // transmit a float value
{

	// turn the negative float into a postive number
	// and check if the precision for negative values
	// is higher than six because of the negative sign
	// in the number -9999999 you need at least one
	// digit after the decimal point.
	
	unsigned char negativeSign = 0;
	
	if (fnum < 0)
	{
		negativeSign = 1;
		fnum *= -1;
		if (precision > 6)
			precision = 6;
	}
	
	float multiplier;
	
	// positive numbers can have higher precision
	switch (precision)
	{
		case 0:
		multiplier = 1.0;
		break;
		case 1:
		multiplier = 10.0;
		break;
		case 2:
		multiplier = 100.0;
		break;
		case 3:
		multiplier = 1000.0;
		break;
		case 4:
		multiplier = 10000.0;
		break;
		case 5:
		multiplier = 100000.0;
		break;
		case 6:
		multiplier = 1000000.0;
		break;
		case 7:
		multiplier = 10000000.0;
		break;
		default: // if precision is more than 7 default to 1 
		multiplier = 1.0;
		precision = 0;
		break;
	}
	
	long num = (long)(fnum * multiplier);
	
	unsigned char digit;
	
	unsigned char value;
	
	unsigned char negativeFlag = 0;
	
	unsigned char decimalPos = precision + 1; // altered to calculate decimal position
	
	for (digit = 1; digit <= no_of_digits; digit++)
	{
		value = num % 10;
		
		// Enable zero blanking from the first digit after the decimal point.
		// If the number is negative then the second digit after the decimal
		// point is a negative sign. Thus the negative sign will always follow
		// the two digits after the decimal point.
		if (num == 0 && digit > decimalPos)
		{			 
			if (negativeSign && !negativeFlag)
			{
				value = NEGATIVE_DIGIT;
				negativeFlag = 1;
			}
			else
				value = BLANK_DIGIT;
		}
			
		num /= 10;
		
		// include the decimal point when transmitting the first digit after the decimal
		// disregard the decimal point if no decimal is required
		if (digit == decimalPos && precision != 0)
				value |= 128; 
			
		transmit(digit, value);
	}
}

void MAX7221shift::transmit(unsigned char address, unsigned char data)
{
	unsigned int packet = address << 8 | data;
	
	unsigned char i;
	
	digitalWrite(loadPin, LOW); // set load LOW for the MAX7219/7221 to receive
	
	for (i = 0; i < 16; i++)
	{
		digitalWrite(dataPin, (packet & (1 << (15 - i))));
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);	
	}
	
	digitalWrite(loadPin, HIGH); // load the value
}