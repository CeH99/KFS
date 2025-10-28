#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

const int defaultPos = 0;
String lastMessage = ""; // хранит последнее сообщение на экране

void turnRight();
void turnLeft();
void showMessage(const String& msg);

class TemperatureSensor {
private:
  int pin_;
  int lastValue_;

public:
  TemperatureSensor(int pin) : lastValue_(0), pin_(pin) {
    pinMode(pin_, INPUT);
  }

  void read() {
    lastValue_ = analogRead(pin_);
  }

  int getValue() {
    return lastValue_;
  }
};

class Controller {
private:
  Servo servo_;
  LiquidCrystal_I2C* lcd_;
  TemperatureSensor tmp_;

public:
  Controller(Servo& servo, LiquidCrystal_I2C* lcd, TemperatureSensor& tmp)
    : servo_(servo), lcd_(lcd), tmp_(tmp) {}

  void begin() {
    lcd_->init();
    lcd_->backlight();
    servo_.attach(9, 500, 2500);
    servo_.write(defaultPos);
  }

  void run() {
    tmp_.read();
    // Poka chto nicho
  }
};

Servo servo;
LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x27, 16, 2);
TemperatureSensor tmp(9);
Controller controller(servo, lcd, tmp);

int currentTurn = 0;

// === Приём команд от мастера (Pico W) ===
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char command = Wire.read();
    if (command == 'L') {
      turnLeft();
    } else if (command == 'R') {
      turnRight();
    }
  }
}

// === Настройка ===
void setup() {
  Serial.begin(9600);

  // Настройка I2C (адрес 0x08)
  Wire.begin(0x08); // Arduino как slave
  Wire.onReceive(receiveEvent);

  controller.begin();
  showMessage("Ready");
}

// === Основной цикл ===
void loop() {
  controller.run(); // можно оставить для температуры
  delay(100);
}

// === Управление сервой ===
void turnRight() {
  if (currentTurn >= 90 && currentTurn < 181) {
    servo.write(currentTurn - 90);
    currentTurn -= 90;
    showMessage("Turning Right...");
    delay(1500);
    showMessage("");
  } else {
    showMessage("Cannot turn now");
    delay(500);
  }
}

void turnLeft() {
  if (currentTurn < 180 && currentTurn >= 0) {
    servo.write(currentTurn + 90);
    currentTurn += 90;
    showMessage("Turning Left...");
    delay(1500);
    showMessage("");
  } else {
    showMessage("Cannot turn now");
    delay(500);
  }
}

// === Отображение сообщений ===
void showMessage(const String& msg) {
  if (msg != lastMessage) {
    lcd->clear();
    lcd->print(msg);
    lastMessage = msg;
  }
}
