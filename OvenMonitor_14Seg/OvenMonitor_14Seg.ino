/*************************************************** 
  Oven Temperature Monitor 
  with MAX31855 demo board MAX31855PMB1, 
  Teensy 3.X, and Adafruit 14-segment display

  Based on example written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /**
  *  What this program does
  *  Measures temp once/second
  *  Calcs drift around a guessed setpoint
  *  
  *  Example: oven is at room temp 70F, turns on and ramps to 400F, then varies from 370 to 420
  *  
  *  When the temp passes the setpoint either overshooting or undershooting, then reverses direction,
  *  we can detect those as local maxima and minima. If the temperature is ramping up or down to a 
  *  new setpoint, there will only be an initial maximum or minimum but not both, the other will be still
  *  changing.
  *  
  *  If we look at the derivate of the curve of temp with respect to time, dtemp/dtime, when ramping
  *  to a new setpoint the slope will have the same sign and roughly the same value. When reaching
  *  a local maximum or minimum, the slope decreases to zero, then changes direction.
  *  
  *  When the oven door is opened, there will be a sudden decrease in temperature which might look similar
  *  to a local maximum, but will likely have a steeper slope and a deeper minimum, which would let using
  *  tell a door open from a normal temperature deviation.
  *  
  *  The setpoint should be roughly the mean of the local maxima and minima. If the control overshoot and 
  *  undershoot are symmetrical, then the setpoint should be the point of inflection (where the sign of
  *  the first derivate changes). It would be interesting to see if this is indeed the case.
  *  
  */

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

/**
 * These are 14-segment displays so... 
 * There are 14 digit segments plus the decimal point for 15 LEDs total per digit
 * 0x01 
 * 0x02 
 * 0x04 
 * 0x08 
 * 0x10 
 * 0x20 
 * 0x40 
 * 0x80 
 * dec point
 */
 
	double f_now;
	double f_min, f_max;
	double f_min_drift, f_max_drift;	// drifting around setpoint

void setup() {

 while ((!Serial) && (millis() < 5000));    // wait until serial monitor is open or timeout

  Serial.begin(115200);
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(1000);

  alpha4.begin(0x70);  // pass in the address
  
  alpha4.clear();
  alpha4.writeDisplay();
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
  
  f_now = thermocouple.readFarenheit();
  f_min = f_now;
  f_max = f_now;
  
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

   f_now = thermocouple.readFarenheit();
   Serial.println(f_now);
   
/**
 *  Update min and max but we want these to self-adjust around the mean
 *  and not get confused by oven turn-on, which would have the min temp
 *  stuck at room temp. So how do we tell we are ramping up to the set point
 *  vs drifting around it?
 *  
 *  If temp peaks and then drops, by maybe 5 deg, we've reached a value
 *  higher than the setpoint, so reset the min.
 *  
 */
   if (f_max < f_now) f_max = f_now;
   if (f_min > f_now) f_min = f_now;
   
   
   
   
   
   
   
   Serial.println(f_now);
   
   display_temp_f (f_min, '-');
   delay(1000);
   display_temp_f (f_now, 'F');
   delay(1000);
   display_temp_f (f_max, '+');
   delay(1000);
   


  delay(1000);
}

void display_temp_f (double degf, char last)
{
   // just display the integer part on a 4-segment LED display
//   f = 325.55;  // dummy data to test
   uint16_t temp_in_f = (uint16_t) degf;     

  if (degf < 0)
  {
    Serial.print("Temp is negative:");
    Serial.println(degf);
    degf = 0;  // TODO make neg temps work
  }

   if (degf > 999) 
    {
      Serial.print("Clipping! Temp exceeds 999F:");
      Serial.println(degf);
      degf = 999;
    }
   // temp is 0..999
   Serial.println(degf);
   
  uint8_t hundreds = degf/100;   // 0..9

  Serial.print("Hundreds=");
  Serial.println(hundreds);

  if (hundreds > 0)
  {
    alpha4.writeDigitAscii(0, hundreds+0x30);
  }
  else alpha4.writeDigitAscii(0, ' ');    // ascii space is 0x32

  degf = degf - (hundreds * 100);  // now temp is 0..99

  uint8_t tens = degf/10;  // 0..9
  
  alpha4.writeDigitAscii(1, tens+0x30);

  degf = degf - (tens * 10);  // now temp is 0..9

  alpha4.writeDigitAscii(2, degf+0x30);

  alpha4.writeDigitAscii(3, last);
  // alpha4.writeDigitAscii(1, '-');
  // alpha4.writeDigitAscii(2, '+');
  alpha4.writeDisplay();
  }


