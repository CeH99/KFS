#include <Arduino.h>


void setup() {
  for(int i = 0; i < 30; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  for(int i = 0; i < 30; i++) {
    digitalWrite(i, HIGH);
  }
}

