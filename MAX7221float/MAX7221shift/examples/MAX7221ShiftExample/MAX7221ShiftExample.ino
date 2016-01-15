#include <MAX7221shift.h>

/* This example code reads an analogue value on pin 0
   and displays it as a voltage on the MAX7221.
   It also boots up in test mode for 2 seconds to
   indicate that all the segments are functioning
   normally. It then uses the same analogue value to
   vary the brightness of the display using the 
   setBrightness() method.
	 
   The following methods are available:
	 
   initialize(unsigned char _clockPin, unsigned char _loadPin, unsigned char _dataPin, unsigned char _no_of_digits)
   Initilizes the MAX7219/7221 for normal operation.
   MAX7221shift does not make use of the SPI library so
   you can call as many instances of the MAX7221shift class  
   with different pin selections as you want.
   The clockPin, loadPin and dataPin are initialized for you.
   shutdown(OFF), testDisplay(NORMAL) and setBrightness(15)
   are called for normal operation.
  
   The following methods can be called independently
   from initialize():
	 
   setBrightness(unsigned char brightness)
   Any value between 1 and 15, default is 15.
	 
   shutdown(unsigned char choice)
   Two choices ON or OFF, default is OFF. 
   The method shuts down the display but
   you can still write to the MAX7221.
   This is an internal funtion of the MAX7221.
   testDisplay() will overwrite the shutdown() function.
	 
   testDisplay(unsigned char choice)
   Two choices TEST or NORMAL, default is NORMAL.
   All digits are turned on at full brightness.
   This is an internal funtion of the MAX7219/7221.
	 
   print(float fnum, unsigned char precision)
   Displays the float number with the required precision.
   The decimal point will vary according to the precision value.
   Leading zero blanking always follows the digit after 
   the decimal point for positive numbers and after the 
   negative sign for negative numbers.	 
*/

MAX7221shift myDisplay;

const float ratio = (5.0/1023.0);

void setup()
{
  myDisplay.initialize(2, 3, 4, 8); // (clockPin, loadPin, dataPin, no_of_digits)
  myDisplay.testDisplay(TEST); // turn all the digits on
  delay(2000);
  myDisplay.testDisplay(NORMAL); // back to normal
}

void loop()
{
  unsigned int adc = analogRead(A0);
  float voltage = ratio * adc;
  unsigned int brightness = adc/68; // 1023/68 = 15.04
  myDisplay.setBrightness(brightness);
  myDisplay.print(voltage, 2); // print the voltage data with a precision of 2 decimal points
}

