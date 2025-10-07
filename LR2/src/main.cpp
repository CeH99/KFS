#include <Arduino.h>
#include <memory>
#define DEBUG

// --------- Interfaces ---------
class IStratagy
{
  public:
    virtual ~IStratagy() {}
    
    virtual int compute(int light, bool motion) = 0;
};

class ISensor
{
  protected: 
    uint8_t pin_;
  public:
    ISensor() : pin_(0) {}
    ISensor(uint8_t pin) : pin_(pin)
    {   
        pinMode(pin_, INPUT);
    }
    virtual ~ISensor() {}

    virtual int read() { 
      return analogRead(pin_); 
    }
};

// --------- Led ---------
class Led
{
  private:
    uint8_t pin_;         
    bool state_;       
    uint32_t brightness_; 
  public:

    Led(int pin) : pin_(pin), state_(LOW), brightness_(0)
    {
        pinMode(pin_, OUTPUT);  
    }
    ~Led() {}

    void on()
    {
        state_ = HIGH;
        digitalWrite(pin_, state_);
    }

    void off()
    {
        state_ = LOW;
        digitalWrite(pin_, state_);
    }

    void setBrightness(uint32_t brightness)
    {
        brightness_ = brightness;
        analogWrite(pin_, brightness_);
    }
};

// --------- Sensors ---------

class LightSensor : public ISensor
{
  public:
    LightSensor(uint8_t pin) : ISensor(pin) {}
    ~LightSensor() {}
    int read() override { return ISensor::read(); }
};

class MoveSensor : public ISensor
{
  public:
    MoveSensor(uint8_t pin) : ISensor(pin) {}
    ~MoveSensor() {}

    int read() override { return digitalRead(pin_); }
};

// --------- Strategies ---------

class EnergySavingStratagy : public IStratagy
{
  public:
      int compute(int light, bool motion) override
      {
        if(light < 900 && light > 500 && motion)
          return 150;
        else if(light < 500 && motion)
          return 100;
        else
          return 0;
      }
};

class ComfortStratagy : public IStratagy
{
  public:
    int compute(int light, bool motion) override
    {
      if(light >= 900 && motion)      
        return 255;
      else if(light < 900 && light > 500 && motion)
        return 200;
      else if(light <= 500 && motion)
        return 120;
      else
        return 0;
    }
};

// --------- Factory ---------
class StrategyFactory {
public:
    static IStratagy* createStrategy(String mode) {
        if (mode == "energy") return new EnergySavingStratagy();
        if (mode == "comfort") return new ComfortStratagy();
        return nullptr;
    }
};


// --------- Controller ---------

class LightingConroller
{
  Led* led_;
  ISensor* lightSensor_;
  ISensor* moveSensor_;
  IStratagy* stratagy_;
  public:
    LightingConroller(Led* led, ISensor* lightSensor, ISensor* moveSensor, IStratagy* stratagy) 
      : led_(led), lightSensor_(lightSensor), moveSensor_(moveSensor), stratagy_(stratagy) 
    {}
    ~LightingConroller() {}

    void update()
    {
      if(!stratagy_) return;

      int light = lightSensor_->read();
      bool motion = !(moveSensor_->read() > 0);   
      
      #ifdef DEBUG
      Serial.print("Light: ");
      Serial.print(light);
      Serial.print(" Motion: ");
      Serial.println(motion);
      Serial.print("Brightness: ");
      Serial.println(stratagy_->compute(light, motion));
      Serial.println("-----");
      #endif

      led_->setBrightness(stratagy_->compute(light, motion));
    }
};

Led led(0);
LightSensor lightSensor(26);
MoveSensor moveSensor(27);
std::unique_ptr<IStratagy> strategy(StrategyFactory::createStrategy("comfort"));
LightingConroller controller(&led, &lightSensor, &moveSensor, strategy.get());


void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

void loop()
{
  controller.update();
  delay(1000);
}
