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

// Include the lowest level I2C library
#if defined (__MK20DX256__) || defined (__MK20DX128__) 	// Teensy 3.1 or 3.2 || Teensy 3.0
#include <i2c_t3.h>		
#else
#include <Wire.h>	// for AVR I2C library
#endif

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
  
  alpha4.setBrightness(0x07);		// 15 is max bright
  
}

/**
These make the loop time consistent by adjusting one of the delay()
*/
uint32_t time_old, time_now, time_loop, loop_tweak=200;

void loop() {
  // print temp every 5 seconds along with min and max
  time_old = time_now;  // about 5 seconds ago
  time_now = millis();  // current time
  Serial.print("@ ");
  Serial.print(time_now/1000);
   Serial.print(" Die= ");
   Serial.print(thermocouple.readInternal());
   Serial.print(" C, ");

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("TC= "); 
   Serial.print(c);
	 Serial.print(" C | ");
   }
   f_now = thermocouple.readFarenheit(); 
    if ((f_max > 100) && (f_now < 5)) 
    {
      // probably an oven, and there is a bug in thermocouple.readFarenheit()
      Serial.println();
      Serial.print("Error in thermocouple.readFarenheit(): ");
      Serial.println(f_now);
    }
   Serial.print(f_now);    // ??? sometimes prints 0.00
   Serial.print(" F, ");
   
/**
 *  Update min and max but we want these to self-adjust around the mean
 *  and not get confused by oven turn-on, which would have the min temp
 *  stuck at room temp. So how do we tell we are ramping up to the set point
 *  vs drifting around it?
 *  
 *  If temp peaks and then drops, by maybe 5 deg, we've reached a value
 *  higher than the setpoint, so reset the min.
 *  
 *  ??? If f_now is really 0.00 why does f_min not get set to 0?
 */
   if (f_max < f_now) f_max = f_now;
   if (f_min > f_now) f_min = f_now;
   
   display_temp_f (f_now, 'F');
   delay(3000 - loop_tweak);   // deduct some process time 0 min to 1000 perhaps
   display_temp_f (f_min, '-');
   delay(1000);   
   display_temp_f (f_max, '+');
   delay(1000);
   
   Serial.print("min/max = ");
   Serial.print(f_min);
   Serial.print(" / ");
   Serial.print(f_max);

  // 5 seconds through loop, plus processing time
   
   Serial.print(" loop= ");
   Serial.println(time_loop);

   time_loop = millis() - time_now;
   if (time_loop != 5000)
   {
      time_loop > 5000 ? loop_tweak++ : loop_tweak-- ;
    }
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
//   Serial.println(degf);
   
  uint8_t hundreds = degf/100;   // 0..9

  // debug
//  Serial.print("Hundreds=");
//  Serial.println(hundreds);

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


