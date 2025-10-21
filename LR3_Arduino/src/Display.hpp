#pragma once

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

class Display {
private:
    LiquidCrystal_I2C lcd_;

public:
    Display(uint8_t address, uint8_t cols, uint8_t rows) : lcd_(address, cols, rows) {}

    void begin() {
        lcd_.init();
        lcd_.backlight();
    }

    void showStartupAnimation() {
        lcd_.clear();
        lcd_.setCursor(0, 0);
        lcd_.print("System starting");
        for (int i = 0; i < 5; ++i) {
            lcd_.setCursor(i + 1, 1);
            delay(500);
            lcd_.print(".");
        }
        delay(500);
        lcd_.clear();
    }

    void showData(float temperature, float humidity) {
        lcd_.clear();
        lcd_.setCursor(0, 0);
        lcd_.print("Temp: ");
        lcd_.print(temperature, 1);
        lcd_.print((char)223);
        lcd_.print("C");

        lcd_.setCursor(0, 1);
        lcd_.print("Hum:  ");
        lcd_.print(humidity, 1);
        lcd_.print("%");
    }

    void showError(const char* message) {
        lcd_.clear();
        lcd_.setCursor(0, 0);
        lcd_.print("Error:");
        lcd_.setCursor(0, 1);
        lcd_.print(message);
    }
};