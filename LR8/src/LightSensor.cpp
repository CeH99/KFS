#include "S:\PlatformIO\LR7\lib\LightSensor.h"

LightSensor::LightSensor(int pin) {
    _pin = pin;
}

void LightSensor::begin() {
    pinMode(_pin, INPUT);
}

int LightSensor::getValue() {
    //(0-4095)
    return analogRead(_pin);
}