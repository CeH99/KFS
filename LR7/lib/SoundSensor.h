#ifndef SOUND_SENSOR_H
#define SOUND_SENSOR_H

#include <Arduino.h>

class SoundSensor {
public:
    SoundSensor(int pin);

    void begin();

    int getValue();

private:
    int _pin; 
};

#endif