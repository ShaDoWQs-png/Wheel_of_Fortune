#include "Wait.h"

void wait(int dur) {
  unsigned long preTime = millis();

  while(millis() - preTime < (unsigned long)dur) {}

  return;
}