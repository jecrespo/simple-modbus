#include <SPI.h>
#include <MAX7221spiV2.h>


MAX7221spi myDisplay;

void setup()
{
  myDisplay.initialize(4); // number of digits used
  myDisplay.testDisplay(TEST); // turn all the digits on
  delay(2000);
  myDisplay.testDisplay(NORMAL); // back to normal
}

void loop()
{
  myDisplay.print(678, 0); 
  delay(2000);
  myDisplay.print(6.45, 2); 
  delay(2000);
  myDisplay.print(-6.45, 2); 
  delay(2000);
  myDisplay.print(-6.4, 1); 
  delay(2000);
}

