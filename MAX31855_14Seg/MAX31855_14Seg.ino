/*************************************************** 
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5
// For Teensy 3
//#define MAXDO   12
//#define MAXCS   9
//#define MAXCLK  13

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Example creating a thermocouple instance with hardware SPI (Uno/Mega only)
// on a given CS pin.
//#define MAXCS   9
//Adafruit_MAX31855 thermocouple(MAXCS);

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {

 while ((!Serial) && (millis() < 5000));    // wait until serial monitor is open or timeout

  Serial.begin(115200);
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);

  alpha4.begin(0x70);  // pass in the address
//
//  // turn on all segments left to right
//  alpha4.writeDigitRaw(3, 0x0);
//  alpha4.writeDigitRaw(0, 0xFFFF);
//  alpha4.writeDisplay();
//  delay(200);
//  alpha4.writeDigitRaw(0, 0x0);
//  alpha4.writeDigitRaw(1, 0xFFFF);
//  alpha4.writeDisplay();
//  delay(200);
//  alpha4.writeDigitRaw(1, 0x0);
//  alpha4.writeDigitRaw(2, 0xFFFF);
//  alpha4.writeDisplay();
//  delay(200);
//  alpha4.writeDigitRaw(2, 0x0);
//  alpha4.writeDigitRaw(3, 0xFFFF);
//  alpha4.writeDisplay();
//  delay(200);

  alpha4.writeDigitAscii(0, 'M');
  alpha4.writeDigitAscii(1, 'A');
  alpha4.writeDigitAscii(2, 'X');
  alpha4.writeDigitAscii(3, '3');
  alpha4.writeDisplay();
  delay(500);

  alpha4.writeDigitAscii(0, '1');
  alpha4.writeDigitAscii(1, '8');
  alpha4.writeDigitAscii(2, '5');
  alpha4.writeDigitAscii(3, '5');
  alpha4.writeDisplay();
  delay(500);
  
  alpha4.clear();
  alpha4.writeDisplay();
  
}

void loop() {
  // basic readout test, just print the current temp
   Serial.print("Internal Temp = ");
   Serial.println(thermocouple.readInternal());

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("C = "); 
     Serial.println(c);
   }
   Serial.print("F = ");

   double f = thermocouple.readFarenheit();
   Serial.println(f);

   // just display the integer part on a 4-segment LED display
//   f = 325.55;  // dummy data to test
   uint16_t temp_in_f = (uint16_t) f;     

  if (temp_in_f < 0)
  {
    Serial.print("Temp is negative:");
    Serial.println(temp_in_f);
    temp_in_f = 0;  // TODO make neg temps work
  }

   if (temp_in_f > 999) 
    {
      Serial.print("Clipping! Temp exceeds 999F:");
      Serial.println(temp_in_f);
      temp_in_f = 999;
    }
   // temp is 0..999
   Serial.println(temp_in_f);
   
  uint8_t hundreds = temp_in_f/100;   // 0..9

  Serial.print("Hundreds=");
  Serial.println(hundreds);

  if (hundreds > 0)
  {
    alpha4.writeDigitAscii(0, hundreds+0x30);
  }
  else alpha4.writeDigitAscii(0, ' ');    // ascii space is 0x32

  temp_in_f = temp_in_f - (hundreds * 100);  // now temp is 0..99

  uint8_t tens = temp_in_f/10;  // 0..9
  
  alpha4.writeDigitAscii(1, tens+0x30);

  temp_in_f = temp_in_f - (tens * 10);  // now temp is 0..9

  alpha4.writeDigitAscii(2, temp_in_f+0x30);

  alpha4.writeDigitAscii(3, 'F');
  alpha4.writeDisplay();
  delay(1000);
}


