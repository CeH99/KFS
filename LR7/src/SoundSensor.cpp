#include "S:\PlatformIO\LR7\lib\SoundSensor.h"

SoundSensor::SoundSensor(int pin) {
    _pin = pin;
}

void SoundSensor::begin() {
    pinMode(_pin, INPUT);
}

int SoundSensor::getValue() {
    //(0-4095)
    return analogRead(_pin);
}