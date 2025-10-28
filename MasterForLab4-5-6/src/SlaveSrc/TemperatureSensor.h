#pragma once

class TemperatureSensor {
  int pin_;
  int lastValue_;
public:
  TemperatureSensor(int pin) : pin_(pin), lastValue_(0) {}
  void begin() { pinMode(pin_, INPUT); }
  void read() { lastValue_ = analogRead(pin_); }
  int getValue() const { return lastValue_; }
};