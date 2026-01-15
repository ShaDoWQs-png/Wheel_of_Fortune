/* ----------------------------

WHEEL OF FORTUNE GAME v0.2

Made by Grayson Camara
Oct. 2025 - Dec. 2025

---------------------------- */

//libaries, object definitions, function declarations
#include <LiquidCrystal_I2C.h>
#include "Ext_Functions/Encourager.cpp"
#include "Ext_Functions/SlideText/SlideText.h"
#include "Ext_Functions/Wait/Wait.h"

LiquidCrystal_I2C display(0x27, 16, 2);
Encourager encourage(display);

void wait(int dur);
void diffSelect();
void cycleLEDs();
int randomGen();

//pin definitions (ATTinyCore Arduino pin numbers)
constexpr int DATAPIN = 0;
constexpr int CLKPIN = 10;
constexpr int LATCHPIN = 1;
constexpr int STOPBUTTPIN = 5;
constexpr int BUZZERPIN = 7;
constexpr int RESETPIN = 8;
constexpr int DIFFPOT = A1;

//lcd char definitions
constexpr byte blockIndex = 0;
byte blockArr[8] {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
};

//general variables
int diffLvl;
bool stopButt;
// LEDs are tracked by index instead of by bitmask, prevents wrap bugs
constexpr int LED_COUNT = 24;
constexpr int WIN_INDEX = 15; 
int currentLEDIndex = 0;
unsigned int score = 0;
bool superHard = false;

void setup() {
  delay(200);
  randomSeed(analogRead(DIFFPOT));

  //shift register setup
  pinMode(DATAPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLKPIN, OUTPUT);

  //other component setup
  pinMode(STOPBUTTPIN, INPUT_PULLUP);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(RESETPIN, OUTPUT);
  
  //display setup
  display.init();
  delay(100);
  display.createChar(blockIndex, blockArr);
  delay(50);
  display.backlight();
  delay(50);
  display.setCursor(0, 0);
  delay(50);

  //sliding text setup
  SlideText::setDisplay(display);
}



void loop() {
  //-------DIFFICULTY SELECT-----------------------------------------------

  SlideText::slideText("Turn knob to");
  display.setCursor(0, 1);
  SlideText::slideText("pick difficulty");
  wait(2000);
  display.clear();

  display.setCursor(0, 0);
  SlideText::slideText("Difficulty:");
  
  do {
    stopButt = (digitalRead(STOPBUTTPIN) == LOW);
    diffSelect();
  } while(!stopButt);

  display.clear();
  tone(BUZZERPIN, 1000, 200); //beep to confirm selection
  wait(500);

  //-------GAME INTRO------------------------------------------------------
  
  display.setCursor(0, 0);
  display.print("Ready?");

  for (int i = 0; i < 3; i++) {
    tone(BUZZERPIN, 750, 100);
    wait(750);
  }

  display.clear();
  display.print("Go!");
  tone(BUZZERPIN, 1000, 200);

  //prepare for game start
  delay(1000);
  display.clear();

  //-------GAME START------------------------------------------------------

  display.setCursor(0, 0);
  display.print("Score: ");   //set score display to 0
  display.setCursor(7, 0);
  display.print(score);

  while(score < 10) {   //score checking/led cycle loop
    cycleLEDs();
    encourage.update();

    //check for score increment
    if((currentLEDIndex == WIN_INDEX) && stopButt) {
      score++;

      if(score == 3 || score == 7) encourage.start(1); //write message on line 1, not line 0

      tone(BUZZERPIN, 2000, 50);
      
      //print score to display
      display.setCursor(0, 0);
      display.print("Score: ");
      display.print(score);
        
      // Wait for button release to stop multiple increments
      while(stopButt) {
        stopButt = (digitalRead(STOPBUTTPIN) == LOW);
        delay(10);
      }
    }
  }

  //-------GAME END--------------------------------------------------------

  display.clear(); 
  display.setCursor(0, 0);
  display.print("Wow, great job!");
  display.setCursor(0, 1);
  display.print("See you later...");

  //win song 
  tone(BUZZERPIN, 523, 150);
  delay(100);
  tone(BUZZERPIN, 659, 150);
  delay(100);
  tone(BUZZERPIN, 784, 150);
  delay(100);
  tone(BUZZERPIN, 1046, 300);
  delay(1000);

  display.clear();

  //Reset Arduino
  digitalWrite(RESETPIN, HIGH);
}



//select difficulty and display on 7-seg
void diffSelect() {
  int rawVal = analogRead(DIFFPOT);
  diffLvl = map(rawVal, 0, 1023, 1, 16);
  display.setCursor(0, 1);

  static int lastDiffLvl = -1;
  // Only redraw the difficulty row when the value actually changes to avoid flicker

  if(diffLvl == lastDiffLvl) return;
  lastDiffLvl = diffLvl;

  (diffLvl > 14) ? superHard = true : superHard = false;

  if(superHard) {
    // write the first three characters as blocks
    for(int i = 0; i < 3; i++) {
      display.setCursor(i, 1);
      display.write((byte)blockIndex);
    }

    // print the "SUPER HARD" text starting at column 3
    display.setCursor(3, 1);
    display.print("SUPER HARD");

    // write the last three characters as blocks
    for(int i = 13; i < 16; i++) {
      display.setCursor(i, 1);
      display.write(byte(blockIndex));
    }

    return;
  }

  // non-superHard: draw the bar from column 0
  display.setCursor(0, 1);
  for(int i = 0; i < 16; i++) {
    if(i < diffLvl) {
      display.write(byte(blockIndex));
    } else {
      display.print(" ");
    }
  }
}

//cycle through LEDs based on difficulty setting
void cycleLEDs() {
  stopButt = (digitalRead(STOPBUTTPIN) == LOW);
  static bool scrollReverse = false;

  //make things harder if player says so
  if(superHard) {
    int eventDecision = random(0, 10);

    if(eventDecision == 7) scrollReverse = !scrollReverse;
    if(eventDecision < 2) wait(100); //20% chance to throw off player's timing
  }
  
  if(!stopButt) {
    if(scrollReverse) {
      currentLEDIndex = (currentLEDIndex + LED_COUNT - 1) % LED_COUNT;
    } else {
      currentLEDIndex = (currentLEDIndex + 1) % LED_COUNT;
    }

  } else if (stopButt && (currentLEDIndex != WIN_INDEX)) {
      // While the button is held (active-low), show a random LED to give feedback.
      while(digitalRead(STOPBUTTPIN) == LOW) {
        currentLEDIndex = randomGen();

        // Update the displays immediately to show the random LED while the button is still pressed
        uint32_t tempLED = (1UL << currentLEDIndex);
        byte l = tempLED & 0xFF;
        byte m = (tempLED >> 8) & 0xFF;
        byte h = (tempLED >> 16) & 0xFF;
        digitalWrite(LATCHPIN, LOW);
        shiftOut(DATAPIN, CLKPIN, LSBFIRST, l);
        shiftOut(DATAPIN, CLKPIN, LSBFIRST, m);
        shiftOut(DATAPIN, CLKPIN, LSBFIRST, h);
        digitalWrite(LATCHPIN, HIGH);

        wait(5); // looks like random data (cool)
      }
  }
  
  //int split for shiftOut()
  uint32_t currentLED = (1UL << currentLEDIndex);
  byte lowByte = currentLED & 0xFF;
  byte midByte = (currentLED >> 8) & 0xFF;
  byte highByte = (currentLED >> 16) & 0xFF;

  //write led patterns to shift registers
  digitalWrite(LATCHPIN, LOW);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, lowByte);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, midByte);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, highByte);
  digitalWrite(LATCHPIN, HIGH);

  (!superHard) ? wait(200 - (diffLvl * 10)) : wait(50); //delay based on difficulty
}

//random number generator that avoids winning position
int randomGen() {
  int idx = random(0, LED_COUNT);
  while(idx == WIN_INDEX) {
    idx = random(0, LED_COUNT);
  }
  return idx;
}