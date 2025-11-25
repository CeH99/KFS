#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

class LightSensor {
public:
    LightSensor(int pin);

    void begin();
    int getValue();

private:
    int _pin;
};

#endif 