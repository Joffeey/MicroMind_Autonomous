// IRSensor.h

#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <Arduino.h>

class IRSensor{
    public:
        IRSensor(int _pin) : pin(_pin){
            pinMode(pin, OUTPUT);
        }

        int read() const{
            return analogRead(pin);
        } 
    
    
    
    private:
        int pin;
};

#endif