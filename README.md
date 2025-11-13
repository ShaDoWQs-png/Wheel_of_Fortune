# Wheel of Fortune

This is a project made for Arduino, similar to Wheel of Fortune, where you have to stop a "spinning" LED at a specific spot. PlatformIO was used to upload and compile all code in the project, and was originally built on an Arduino UNO R4 WIFI. 

The *No-encourager.cpp* file doesn't require *Encourager.cpp* and instead stores all the code in that file. I do think it makes the file more cluttered and less readable, but you do you, man. *No-encourager* is also excluded from compilation in *platformio.ini* with an src mask.

## Dependencies:
- Adafruit LiquidCrystal
- PlatformIO
