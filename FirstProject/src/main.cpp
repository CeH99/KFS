#include <Arduino.h>

class Led {
private:
    int pin_;
    bool state_;

public:
    Led(int pin) : pin_(pin), state_(false) {
        pinMode(pin_, OUTPUT);
    }

    void on() {
        digitalWrite(pin_, HIGH);
        state_ = true;
    }

    void off() {
        digitalWrite(pin_, LOW);
        state_ = false;
    }

    void toggle() {
        if (state_) off();
        else on();
    }

    bool isOn() const {
        return state_;
    }
};

class LedController {
private:
    Led* leds_;
    int count_;

public:
    LedController(Led* leds, int count) : leds_(leds), count_(count) {}

    void trafficLight(int redTime, int yellowTime, int greenTime, int blinkTime) {
        enum { RED = 0, YELLOW, GREEN };

        // red
        leds_[RED].on();
        delay(redTime);
        leds_[RED].off();
        blink(RED, 5, blinkTime);

        // yellow
        leds_[YELLOW].on();
        delay(yellowTime);
        leds_[YELLOW].off();

        //green
        leds_[GREEN].on();
        delay(greenTime);
        leds_[GREEN].off();
        blink(GREEN, 5, blinkTime);
    }

private:
    void blink(int index, int times, int time) {
        for (int i = 0; i < times; ++i) {
            leds_[index].on();
            delay(time);
            leds_[index].off();
            delay(time);
        }
    }
};

Led leds[] = { Led(13), Led(12), Led(11) };
LedController controller(leds, 3);

void setup() {
    Serial.begin(9600);
}

void loop() {
    controller.trafficLight(6000, 2000, 6000, 400);
    delay(200);
}