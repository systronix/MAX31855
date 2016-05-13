# MAX31855
Maxim 31855 Cold-Junction Compensated Thermocouple-to-Digital Converter Application Programs (not a library).
These all use Teensy 3.X as the controller but any Arduino should work.

Hardware used
 - Teensy 3.X
 - Adafruit 14-segment 2-digit 0.54" tall [LED display](https://www.adafruit.com/products/3129)
 - Maxim MAX31855PMB1 thermoouple demo board and K-type thermocouple with high-temperature sheath
 - solderless breadboard such as this [Adafruit half-size](https://www.adafruit.com/products/64)
 - USB power supply to run the combination when not plugged into a PC
 - wire jumpers to connect the display, thermocouple, and Teensy together, like [these](https://www.adafruit.com/products/1954)

## OvenMonitor_14Seg
The purpose of this code is to monitor a home baking oven temperature and specifically
measure how much variation in temperature there is. This means absolute deviation from the 
set point and also how much drift there is about the setpoint. This is a work in progress.
