#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Ext_Functions/Wait/Wait.h"

class Encourager {
  private:
        LiquidCrystal_I2C &_lcd;
        int _line;

  public:
    Encourager(LiquidCrystal_I2C &lcd) : _lcd(lcd) {};

      /**
       * @brief Start showing a random encourager message on the LCD.
       * @param line The LCD line to print to (default 0).
       */
      void start(int line = 0) {
        _line = line;
        int message = random(0, 10);

        _lcd.setCursor(0, line);
        
        switch (message) {
          case 0:
            _lcd.print("Good!");
            break;
          case 1:
            _lcd.print("Nice!");
            break;
          case 2:
            _lcd.print("Great Job!");
            break;
          case 3:
            _lcd.print("Not bad!");
            break;
          case 4:
            _lcd.print("Cool!");
            break;
          case 5:
            _lcd.print("I could do that!");
            break;
          case 6:
            _lcd.print("Wow!");
            break;
          case 7:
            _lcd.print("So Graceful!");
            break;
          case 8:
            _lcd.print("Excellent!");
            break;
          case 9:
            _lcd.print("What Timing!");
            break;
          }

          wait(2000); //display for 2 seconds

          _lcd.setCursor(0, _line);
          _lcd.print("                ");

          return;
      };
};