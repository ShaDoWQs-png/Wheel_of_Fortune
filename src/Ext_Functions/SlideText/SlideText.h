#pragma once
#include <LiquidCrystal_I2C.h>

namespace SlideText{
    void setDisplay(LiquidCrystal_I2C& _lcd);
    void slideText(String txt);
}