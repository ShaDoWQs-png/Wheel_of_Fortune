/* ----------------------------

WHEEL OF FORTUNE GAME

Made by Grayson Camara
Nov. 2025

---------------------------- */

//libaries, object definitions, function declarations
#include <Adafruit_LiquidCrystal.h>
#include <Adafruit_GFX.h>

Adafruit_LiquidCrystal display(0x27);

void diffSelect();
void cycleLEDs();
void encouragePlayer(bool clear = false);
int randomGen();

//pin definitions
constexpr int DATAPIN = 12;
constexpr int CLKPIN = 11;
constexpr int LATCHPIN = 10;
constexpr int STOPBUTTPIN = 13;
constexpr int DIFFPIN = 6;
constexpr int BUZZERPIN = 7;
constexpr int RESETPIN = 8;
constexpr int DIFFPOT = A3;

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
uint32_t currentLED = 1;
unsigned int score = 0;
bool superHard = false;

void setup() {
  digitalWrite(RESETPIN, HIGH);  // Stop arduino from looping forever

  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  //shift register setup
  pinMode(DATAPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLKPIN, OUTPUT);

  //other component setup
  pinMode(STOPBUTTPIN, INPUT_PULLUP);
  pinMode(DIFFPIN, INPUT_PULLUP);
  pinMode(DIFFPOT, INPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(RESETPIN, OUTPUT);
  
  //display setup
  display.createChar(blockIndex, blockArr);
  display.begin(16, 2);
  display.setBacklight(HIGH);
  display.setCursor(0, 0);
}



void loop() {
  //-------DIFFICULTY SELECT-----------------------------------------------

  display.print("Turn knob to");
  display.setCursor(0, 1);
  display.print("pick difficulty");
  delay(2000);
  display.clear();
  tone(BUZZERPIN, 1000, 50); //beep to signal start of selection

  display.setCursor(0, 0);
  display.print("Difficulty:");
  
  do {
    stopButt = !digitalRead(STOPBUTTPIN);
    diffSelect();
  } while(!stopButt);

  display.clear();
  tone(BUZZERPIN, 1000, 200); //beep to confirm selection
  delay(500);

  //-------GAME INTRO------------------------------------------------------
  
  
  display.setCursor(0, 0);
  display.print("Ready?");

  for (int i = 0; i < 3; i++) {
    tone(BUZZERPIN, 750, 100);
    delay(750);
  }

  display.clear();
  display.print("Go!");
  tone(BUZZERPIN, 1000, 200);

  Serial.println("Difficulty set to: " + String(diffLvl));

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

    //check for score increment
    if((currentLED & 0x8000) && stopButt) {
      score++;
      encouragePlayer(true); //clear previous message

      if(score == 3 || score == 7) encouragePlayer();

      tone(BUZZERPIN, 2000, 50);
      Serial.println("Score: " + String(score));
      
      display.setCursor(7, 0);  //print score to display
      display.print(score);   
        
      // Wait for button release to stop multiple increments
      while(stopButt) {
        stopButt = !digitalRead(STOPBUTTPIN);
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

  //reset Arduino
  digitalWrite(RESETPIN, HIGH);
}



//select difficulty and display on 7-seg
void diffSelect() {
  int rawVal = analogRead(DIFFPOT);
  diffLvl = map(rawVal, 0, 1023, 1, 16);
  display.setCursor(0, 1);
  Serial.println("Difficulty level: " + String(diffLvl));

  (diffLvl == 16) ? superHard = true : superHard = false;

  if(superHard) {
    for(int i = 0; i < 16; i++) {
      display.write(byte(blockIndex));
    }

    display.setCursor(3, 1);
    display.print("SUPER HARD");
    return;
  }

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
  stopButt = !digitalRead(STOPBUTTPIN);
  static bool scrollReverse = false;

  //make things harder if player says so
  if(superHard) {
    int eventDecision = random(0, 10);

    if(eventDecision == 7) scrollReverse = !scrollReverse;
    if(eventDecision < 2) delay(50); //20% chance to throw off player's timing

    Serial.println(String(eventDecision) + " / " + String(scrollReverse));    //print for debugging
  }
  
  if(!stopButt) {
    (scrollReverse) ? currentLED >>= 1 : currentLED <<= 1;
    if(currentLED == 0) currentLED = 0x80000;
    if(currentLED > 0x800000) currentLED = 1;

  } else if (stopButt && (currentLED & 0x8000) == 0) {
    currentLED = randomGen();
  }
  
  //int split for shiftOut()
  byte lowByte = currentLED & 0xFF;
  byte midByte = (currentLED >> 8) & 0xFF;
  byte highByte = (currentLED >> 16) & 0xFF;

  //write led patterns to shift registers
  digitalWrite(LATCHPIN, LOW);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, lowByte);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, midByte);
  shiftOut(DATAPIN, CLKPIN, LSBFIRST, highByte);
  digitalWrite(LATCHPIN, HIGH);

  (!superHard) ? delay(300 - (diffLvl * 10)) : delay(75); //delay based on difficulty
}   

//random number generator that avoids winning position
int randomGen() {
  int val = random(1, 0x800000);

  while(val & 0x8000) {
    val = random(1, 0x800000);
  }
  return val;
}

//let player know they're a cool person :)
void encouragePlayer(bool clear) {
  int message = random(0, 10);

  display.setCursor(0, 1);

  if(clear) {
    display.print("                ");
    return;
  } 

  switch (message) {
    case 0:
      display.print("Good!");
      break;
    case 1:
      display.print("Nice!");
      break;
    case 2:
      display.print("Great Job!");
      break;
    case 3:
      display.print("Not bad!");
      break;
    case 4:
      display.print("Cool!");
      break;
    case 5:
      display.print("I could do that!");
      break;
    case 6:
      display.print("Wow!");
      break;
    case 7:
      display.print("So Graceful!");
      break;
    case 8:
      display.print("Excellent!");
      break;
    case 9:
      display.print("What Timing!");
      break;
  }
}