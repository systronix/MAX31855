# MAX31855
Maxim 31855 Cold-Junction Compensated Thermocouple-to-Digital Converter Application Programs (not a library).
These all use Teensy 3.X as the controller but any Arduino should work.

## OvenMonitor_14Seg
This uses an Adafruit 2-digit 14-segment 0.54" tall LED display.
The purpose of this code is to monitor a home baking oven temperature and specifically
measure how much variation in temperature there is. This means absolute deviation from the 
set point and also how much drift there is about the setpoint.
