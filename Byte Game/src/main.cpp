#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Button {
private:
    int pin_;
public:
    Button(int pin) : pin_(pin) {
        pinMode(pin_, INPUT_PULLUP);
    }

bool isPressed() const {
    return digitalRead(pin_) == LOW;
}

};

class Led {
private:
    int pin_;
public:
    Led(int pin) : pin_(pin) {
        pinMode(pin_, OUTPUT);
        off();
    }

void on() const {
    digitalWrite(pin_, HIGH);
}

void off() const {
    digitalWrite(pin_, LOW);
}

};

class BinaryGuessGame {
private:
    LiquidCrystal_I2C lcd_;
    Button button0_;
    Button button1_;
    Led ledGreen_;
    Led ledRed_;

    int targetNumber_;
    int userValue_;
    int bitCount_;

public:
    BinaryGuessGame(LiquidCrystal_I2C& lcd, int button0, int button1, int ledGreen, int ledRed)
    : lcd_(lcd), button0_(button0), button1_(button1), ledGreen_(ledGreen), ledRed_(ledRed)
    {
        targetNumber_ = 0; 
        userValue_ = 0;
        bitCount_ = 0; 
    }

void begin() {
    lcd_.init();
    lcd_.backlight();
    randomSeed(analogRead(A0));
    resetGame();
}

void update() {
    ledGreen_.off();
    ledRed_.off();

    if (button0_.isPressed()) {
        addBit(0);
    }
    if (button1_.isPressed()) {
        addBit(1);
    }

    if (bitCount_ >= 4) {
        checkAnswer();
        delay(2000);
        resetGame();
    }
}

private:

void resetGame() {
    userValue_ = 0;
    bitCount_ = 0;
    lcd_.clear();

    targetNumber_ = random(0, 16);
    lcd_.setCursor(0, 0);
    lcd_.print("Dec: ");
    lcd_.print(targetNumber_);
    lcd_.setCursor(0, 1);
    lcd_.print("Bin: ");
}

void addBit(int bit) {
    userValue_ = (userValue_ << 1) | bit;
    bitCount_++;
    lcd_.setCursor(5 + bitCount_, 1);
    lcd_.print(bit);
    delay(300);
}

void checkAnswer() {
    if (userValue_ == targetNumber_) {
        ledGreen_.on();
        lcd_.setCursor(0, 1);
        lcd_.print("Correct!       ");
    } else {
        ledRed_.on();
        lcd_.setCursor(0, 1);
        lcd_.print("Wrong: Bin=    ");
        lcd_.setCursor(11, 1);
        lcd_.print(userValue_, BIN);
    }
}

};

LiquidCrystal_I2C lcd(0x27, 16, 2);
BinaryGuessGame game(lcd, 6, 7, 8, 9);

void setup() {
    game.begin();
}

void loop() {
    game.update();
}
