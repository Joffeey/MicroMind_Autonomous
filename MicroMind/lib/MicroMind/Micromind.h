// Micromind.h
#ifndef MICROMIND_H
#define MICROMIND_H

#include <kitkronikMotorDriver.h>

class Micromind
{
    public:
    Micromind(int leftPin1, int leftPin2, int rightPin1, int rightPin2);
    void forward(int speed);
    void reverse(int speed);
    void stop();
    void turnLeft(int speed);
    void turnRight(int speed);

    private:
    kitkronikMotorDriver leftMotor;
    kitkronikMotorDriver rightMotor;
};

#endif