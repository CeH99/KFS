#include <Arduino.h>

//#define DEBUG 0

class iSensor {
public:
    virtual int readValue() = 0;
};

class Led {
private:
    int pin_;
    bool state_;
    int brightness_;
public:
    Led(int p) : pin_(p), state_(false), brightness_(255) {
        pinMode(pin_, OUTPUT);
    }

public:
    void on() {
        state_ = true;
        analogWrite(pin_, brightness_);
    }

    void off() {
        state_ = false;
        analogWrite(pin_, 0);
    }

    void toggle() {
        state_ = !state_;
        analogWrite(pin_, state_ ? brightness_ : 0);
    }

    void setBrightness(int value) {
        brightness_ = constrain(value, 0, 255); // value (0 - 255)
        if (state_) {
            analogWrite(pin_, brightness_);
        }
    }
};

class LedController {
private:
    Led* leds_;
    int count_;
public:
    LedController(Led* arr, int n) : leds_(arr), count_(n) {}

public:
    void setAllBrightness(int value) {
        for (int i = 0; i < count_; i++) {
            leds_[i].setBrightness(value);
        }
    }

    void blinkSync(int delayMs) {
        for (int i = 0; i < count_; i++) leds_[i].on();
        delay(delayMs);
        for (int i = 0; i < count_; i++) leds_[i].off();
        delay(delayMs);
    }

    void blinkAsync(int delayMs) {
        for (int i = 0; i < count_; i++) {
            leds_[i].on();
            delay(delayMs);
            leds_[i].off();
        }
    }
};

class Pot : public iSensor {
private:
    int pin_;
public:
    Pot(int p) : iSensor() {
        pin_ = p;
        pinMode(pin_, INPUT);
    }
    int readValue() override {
        return analogRead(pin_) / 5;
    }
};

class LightSensor : public iSensor {
private:
    int pin_;
public:
    LightSensor(int p) : iSensor() {
        pin_ = p;   
        pinMode(pin_, INPUT);
    }
    int readValue() override {
        return analogRead(pin_);
    }
};

Led leds[3] = {Led{9}, Led{10}, Led{11}};
LedController controller(leds, 3);
LightSensor lightSensor(A0);
Pot pot(A1); 

void setup() {
    Serial.begin(9600);
}

void loop() {
    controller.setAllBrightness(pot.readValue());
#ifdef DEBUG
    Serial.print("Light: ");
    Serial.print(light);
    Serial.print("  Pot: ");
    Serial.print(pot.readValue());
    Serial.print("  Brightness: ");
    Serial.println(pot.readValue(););
#endif
    if (lightSensor.readValue() < 800) {
        controller.blinkSync(300);
    } else {
        controller.blinkAsync(200);
    }
}
