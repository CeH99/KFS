#include <Arduino.h>

class Led
{
  private:
      int pin_;
      bool state_;
  public:
      Led(int pin) : pin_(pin), state_(false)
      {
          pinMode(pin_, OUTPUT);
      }

      void on()
      {
        state_ = true;
        digitalWrite(pin_, 1);
      }

      void off()
      {
        state_ = false;
        digitalWrite(pin_, 0);
      }

      void toggle()
      {
        if(state_)
          off();
        else
          on();
      }

      bool getState()
      {
        return state_;
      }
};

class Button
{
  private:
      int pin_;
      bool lastState_;
      unsigned long int lastPress_;
  public:
      Button(int pin) : pin_(pin), lastState_(HIGH), lastPress_(0)
      {
        pinMode(pin_, INPUT_PULLUP);
      }

  public:
      bool isPressed()
      {
        return digitalRead(pin_) == LOW;
        
      }

      bool wasPressed()
      {
        bool current = digitalRead(pin_);

        if(lastState_ == HIGH && current == LOW && (millis() - lastPress_) > 50)
        {
          lastState_ = current;
          lastPress_ = millis();
          return true;        
        }
          lastState_ = current;
          return false;
      }
};

  Led led(8);
  Button b1(A5);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
   if (b1.wasPressed())
  {
    led.toggle();
    Serial.println("Toggled LED");
  }
  
}