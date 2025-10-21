#pragma once
#include "ClimateSensor.hpp"
#include "Display.hpp"

class ClimateMonitor {
private:
    ClimateSensor& sensor_;
    Display& display_;

    unsigned long prevMillis_ = 0;
    const unsigned long interval_ = 1000;

public:
    ClimateMonitor(ClimateSensor& s, Display& d) : sensor_(s), display_(d) {}

    void begin() {
        Serial.begin(9600);
        sensor_.begin();
        display_.begin();
        display_.showStartupAnimation();
    }

    void update() {
        unsigned long currentMillis = millis();

        if (currentMillis - prevMillis_ >= interval_) {
            prevMillis_ = currentMillis;

            if (!sensor_.readData()) {
                Serial.println("Failed to read from DHT11!");
                display_.showError("DHT11 read fail!");
                return;
            }

            float temp = sensor_.getTemperature();
            float hum = sensor_.getHumidity();

            display_.showData(temp, hum);

            // data to pc
            Serial.print("TEMP:");
            Serial.print(temp, 1);
            Serial.print(";HUM:");
            Serial.println(hum, 1);
        }
    }
};
