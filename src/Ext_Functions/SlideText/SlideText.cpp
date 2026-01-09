#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>
#include "SlideText.h"

namespace SlideText {
    static LiquidCrystal_I2C* lcd = nullptr;

    void setDisplay(LiquidCrystal_I2C& _lcd) {
        lcd = &_lcd;
    }

    void slideText(const char* txt) {
        if(!lcd || !txt) return;

        unsigned long preTime;

        for(unsigned int i = 0; txt[i] != '\0'; i++) {
            lcd->print(txt[i]);

            preTime = millis();
            while(millis() - preTime < 50) {}
        }
    }
}