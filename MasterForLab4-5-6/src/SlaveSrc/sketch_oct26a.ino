#include "Controller.h"
#include "TemperatureSensor.h"

Controller controller;

void receiveEvent(int howMany) {
  controller.onReceiveI2C(howMany);
}

void setup() {
  controller.begin();
}

void loop() {
  controller.run();
}
