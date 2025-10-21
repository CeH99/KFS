#include <Arduino.h>

#include "ClimateSensor.hpp"
#include "Display.hpp"
#include "ClimateMonitor.hpp"

#define DHTPIN 2
#define DHTTYPE DHT11
#define LCD_ADDR 0x27

ClimateSensor sensor(DHTPIN, DHTTYPE);
Display lcd(LCD_ADDR, 16, 2);
ClimateMonitor monitor(sensor, lcd);

void setup() {
    monitor.begin();
    Serial.begin(9600);
}

void loop() {
    monitor.update();
}
