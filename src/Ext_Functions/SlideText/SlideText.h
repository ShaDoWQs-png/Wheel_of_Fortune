#pragma once
#include <LiquidCrystal_I2C.h>

namespace SlideText{
    /**
     * @brief Set the LCD display to use for sliding text.
     * @param _lcd Reference to the LiquidCrystal_I2C display object.
     */
    void setDisplay(LiquidCrystal_I2C& _lcd);

    /**
     * @brief Slide text onto the LCD display one character at a time.
     * @param txt The text to slide onto the display.
     */
    void slideText(String txt);
}