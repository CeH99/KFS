#pragma once

#include <DHT.h>
#include <Arduino.h>

class ClimateSensor {
private:
    DHT dht_;
    float temperature_;
    float humidity_;

public:
    ClimateSensor(uint8_t pin, uint8_t type) : dht_(pin, type), temperature_(0), humidity_(0) {}

    void begin() {
        dht_.begin();
    }

    bool readData() {
        humidity_ = dht_.readHumidity();
        temperature_ = dht_.readTemperature();
        return !(isnan(humidity_) || isnan(temperature_));
    }

    float getTemperature() const { return temperature_; }
    float getHumidity() const { return humidity_; }
};