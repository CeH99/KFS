#include <Arduino.h>

void setup()
{
    Serial.begin(9600);
    pinMode(0, OUTPUT);
}

void loop()
{
    digitalWrite(0, HIGH);
    Serial.println("on");   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(0, LOW);
    Serial.println("off");    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
}