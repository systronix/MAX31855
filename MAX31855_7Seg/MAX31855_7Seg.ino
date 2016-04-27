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
#include "LedControl.h"

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

/**
 * Teensy 3.1 or 3.2 pins
 * Controller needs to tolerate 5V on I/O
 * 
 * Instantiating this clobbers the MAX31855 output. This LedControl lib might not be sharing SPI.
 */
LedControl lc=LedControl(11,13,10,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {

    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens

      /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /** Set the brightness
   *  setIntensity(chip, intensity)
   *  chip is the chip #, 0 if only one
   *  intensity is 0..15
  *
  **/
  lc.setIntensity(0,7);
  /* and clear the display */
  lc.clearDisplay(0);

  
  Serial.begin(9600);
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);

  writeArduinoOn7Segment();
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
   //Serial.print("F = ");
   //Serial.println(thermocouple.readFarenheit());
 
   delay(1000);
}

/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 7... 
 */
void writeArduinoOn7Segment() {
  lc.setChar(0,7,'a',false);
  delay(delaytime);
  lc.setRow(0,6,0x05);
  delay(delaytime);
  lc.setChar(0,5,'d',false);
  delay(delaytime);
  lc.setRow(0,4,0x1c);
  delay(delaytime);
  lc.setRow(0,3,B00010000);
  delay(delaytime);
  lc.setRow(0,2,0x15);
  delay(delaytime);
  lc.setRow(0,1,0x1D);
  delay(delaytime);
  lc.setRow(0,0,0x80);
  
  delay(delaytime*4);
  lc.clearDisplay(0);
} 
