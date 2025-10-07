#include <LiquidCrystal_I2C.h>

#define DEBUG 1

class ISensor
{
public:
    int pin_;
    int lastValue_;
    virtual void read() = 0;
    virtual int getValue() = 0;
    virtual ~ISensor() {}
};

class MotorController
{
private:
    int pin1_;
    int pin2_;
    int enable_;
    int speed_;
    bool state_;
public:
    MotorController(int pin, int pin2, int enable) 
    : pin1_(pin), pin2_(pin2), enable_(enable), speed_(100), state_(false)
{
    pinMode(pin1_, OUTPUT);
    pinMode(pin2_, OUTPUT);
    pinMode(enable_, OUTPUT);
}

void on()
{       
    digitalWrite(pin1_, HIGH);
    digitalWrite(pin2_, LOW);
    analogWrite(enable_, speed_);
    state_ = true;
}

void off()
{
    digitalWrite(pin1_, LOW);
    digitalWrite(pin2_, LOW);
    analogWrite(enable_, 0);    
    state_ = false; 
}

public:
    void setSpeed(int speed)
    {
        if(speed >= 0 && speed <= 255)
        {
            speed_ = speed;
            analogWrite(enable_, speed_);
        }
    }

    bool getState()
    {
        return state_;
    }
};

class Tmp36 : public ISensor
{
public:
    Tmp36(int pin) 
    {
        pin_ = pin;
        lastValue_ = 0;
    }
    void read() override
    {
        lastValue_ = analogRead(pin_);
        #ifdef DEBUG
            Serial.print("lastValue : ");
			Serial.println(lastValue_);            
        #endif
    }

    int getValue() override
    {
        float temperatureC = ((lastValue_ * 5.0 / 1023.0) - 0.5) * 100.0;
        return temperatureC;
    }
};

MotorController motor(3, 4, 5);
Tmp36 tmp36(A0);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void displayInfo();

void setup() {
    lcd.init();
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
}

void loop() {
    tmp36.read();
    int val = tmp36.getValue();

    if(val > 150 && !motor.getState()) {
        motor.on();
    }
    else if(val <= 150 && motor.getState()) {
        motor.off();
    }

    if(motor.getState()) {
        if(val > 350) motor.setSpeed(255);
        else if(val > 200 && val <= 350) motor.setSpeed(150);
        else motor.setSpeed(50);
    }

    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 500) { // обновляем раз в 0.5 секунды
        displayInfo();
        lastUpdate = millis();
    }
}


void displayInfo()
{
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(2, 0);
    lcd.print("TEMPERATURE: ");
    lcd.setCursor(4, 1);
    lcd.print("Tmp36: ");
    lcd.print(tmp36.getValue());
    lcd.print("C*");
}
