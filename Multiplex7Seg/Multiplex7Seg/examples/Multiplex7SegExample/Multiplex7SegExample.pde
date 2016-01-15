#include <Multiplex7Seg.h>

byte digitPins[] = {9, 10, 11, 12}; // LSB to MSB
byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8}; // seg a to g
unsigned int adc;

void setup()
{
/* set(byte enableZB, byte _noOfDigits, byte digitPins, byte segmentPins)
   enableZB activates zero leading blanking on the digits. 1 == ON, 0 == OFF
   digitPins array stores the 7 segment digits from LSB (0 digit) to MSB (4 digit max)
   segmentPins array stores the segments of the digits from a to g
*/
  Multiplex7Seg::set(1, 4, digitPins, segmentPins); // initialize 
}

void loop()
{ 
  adc = 0;
  for (byte i = 0; i < 10; i++) // read the ch0 adc 10 times to average
  {
    adc += analogRead(A0); 
    delay(10); // wait for adc to stabilize
  }
  adc /= 10; // average the adc
  Multiplex7Seg::loadValue(adc); // load any int value
}
