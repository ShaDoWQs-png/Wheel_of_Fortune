#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>
#include "SlideText.h"

namespace SlideText {
    static LiquidCrystal_I2C* lcd = nullptr;

    void setDisplay(LiquidCrystal_I2C& _lcd) {
        lcd = &_lcd;
    }

    void slideText(String txt) {
        if(!lcd) return;

        unsigned long preTime;

        for(int i = 0; i < txt.length(); i++) {
            lcd->print(txt[i]);

            preTime = millis();
            while(millis() - preTime < 50) {}
        }
    }
}