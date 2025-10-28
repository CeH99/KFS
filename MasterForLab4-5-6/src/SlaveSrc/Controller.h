#pragma once

#define DEBUG

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#include "TemperatureSensor.h"

const int SERVO_PIN = 9;
const int DEFAULT_POS = 0;
const unsigned long SERVO_STEP_DELAY = 10;
const unsigned long MESSAGE_TIMEOUT = 1000;
const int CMD_BUFFER_SIZE = 8;

void receiveEvent(int howMany);

class Controller {
  Servo servo_;
  LiquidCrystal_I2C lcd_;
  TemperatureSensor tmp_;

  int currentTurn_ = 0;
  unsigned long lastMessageTime_ = 0;
  char lastMessage_[17] = "";

  volatile char cmdBuffer_[CMD_BUFFER_SIZE];
  volatile int cmdHead_ = 0;
  volatile int cmdTail_ = 0;

public:
  Controller() : lcd_(0x27, 16, 2), tmp_(A0) {}

  void begin() {
#ifdef DEBUG
    Serial.begin(9600);
    delay(200);
    Serial.println("[Controller] Initializing...");
#endif
    Wire.begin(0x08);
    Wire.onReceive(receiveEvent);

    lcd_.init();
    lcd_.backlight();
    servo_.attach(SERVO_PIN, 500, 2500);
    servo_.write(DEFAULT_POS);
    tmp_.begin();

    showMessage("Ready");
#ifdef DEBUG
    Serial.println("[Controller] Ready!");
#endif
  }

  void run() {
    char cmd;
    if (dequeueCmd(cmd)) processCommand(cmd);

    if (lastMessageTime_ && millis() - lastMessageTime_ > MESSAGE_TIMEOUT) {
      lcd_.clear();
      lastMessage_[0] = '\0';
      lastMessageTime_ = 0;
    }
  }

  // ==================== QUEUE ====================
  bool enqueueCmd(char c) {
    int next = (cmdHead_ + 1) % CMD_BUFFER_SIZE;
    if (next == cmdTail_) return false;
    cmdBuffer_[cmdHead_] = c;
    cmdHead_ = next;
    return true;
  }

private:
  bool dequeueCmd(char &c) {
    if (cmdHead_ == cmdTail_) return false;
    c = cmdBuffer_[cmdTail_];
    cmdTail_ = (cmdTail_ + 1) % CMD_BUFFER_SIZE;
    return true;
  }

  void processCommand(char cmd) {
    switch (cmd) {
      case 'L':
        if (currentTurn_ <= 90) {
          turnSmooth(currentTurn_ + 90);
          currentTurn_ += 90;
        } else {
          showMessage("Cannot turn");
#ifdef DEBUG
          Serial.println("Cannot turn left");
#endif
        }
        break;
      case 'R':
        if (currentTurn_ >= 90) {
          turnSmooth(currentTurn_ - 90);
          currentTurn_ -= 90;
        } else {
          showMessage("Cannot turn");
#ifdef DEBUG
          Serial.println("Cannot turn right");
#endif
        }
        break;
#ifdef DEBUG
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        break;
#endif
    }
  }

  void turnSmooth(int target) {
    int current = servo_.read();
    int step = (target > current) ? 1 : -1;
    if(step > 0) showMessage("Turning Left...");
    if(step < 0) showMessage("Turning Right...");
    while (current != target) {
      current += step;
      servo_.write(current);
      delay(SERVO_STEP_DELAY);
    }
#ifdef DEBUG 
    Serial.print("Servo moved to: ");
    Serial.println(target);
#endif
  }

  void showMessage(const char* msg) {
    if (strncmp(lastMessage_, msg, 16) == 0) return;
    lcd_.print(msg);
    strncpy(lastMessage_, msg, 16);
    lastMessage_[16] = '\0';
    lastMessageTime_ = millis();
#ifdef DEBUG
    Serial.print("LCD: ");
    Serial.println(msg);
#endif
  }

public:

  void onReceiveI2C(int howMany) {
    while (Wire.available()) {
      char cmd = Wire.read();
      enqueueCmd(cmd);
    }
  }
};

