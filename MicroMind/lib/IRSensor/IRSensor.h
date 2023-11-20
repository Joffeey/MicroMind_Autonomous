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
            int res = 0;
            int numOfRead = 20;
            for(int i = 0; i < numOfRead; i++){
                res += analogRead(pin);
            }
            res = res / numOfRead;
            return res;
        } 
    
    
    
    private:
        int pin;
};

#endif