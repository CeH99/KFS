#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);  // Пин PIR
  
}

void loop() {
  int motion = digitalRead(8);
  Serial.println(motion);
  delay(500);
}
