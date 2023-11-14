// Micromind.h
#ifndef MICROMIND_H
#define MICROMIND_H

#include <kitkronikMotorDriver.h>
#include <Arduino.h>

class Micromind
{
    public:

    Micromind(int leftPin1, int leftPin2, int rightPin1, int rightPin2, int hallSensorPin);
    void forward(int speed);
    void reverse(int speed);
    void stop();
    bool turnLeft(int speed);
    bool turnRight(int speed);
    bool turnAround(int speed);
    int readHallSensor();

    private:
    kitkronikMotorDriver leftMotor;
    kitkronikMotorDriver rightMotor;
    int hallSensorPin; 
    unsigned long lastHallSensorTime; //check when we read it last. 
    unsigned long hallSensorInterval; // Check the interval between turns 
    int hallSensorThreshold; //must change this value based on when we wist to stop the rotation. 

};

#endif