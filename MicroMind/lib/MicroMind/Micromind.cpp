// Micromind.cpp
#include <Micromind.h>

Micromind::Micromind(int leftPin1, int leftPin2, int rightPin1, int rightPin2) : leftMotor(leftPin1, leftPin2), rightMotor(rightPin1, rightPin2){}

void Micromind::forward(int speed){
    leftMotor.forward(speed);
    rightMotor.forward(speed);
}

void Micromind::reverse(int speed){
    leftMotor.reverse(speed);
    rightMotor.reverse(speed);
}

void Micromind::stop(){
    leftMotor.stop();
    rightMotor.stop();
}

void Micromind::turnLeft(int speed){
    leftMotor.reverse(speed);
    rightMotor.forward(speed);
}

void Micromind::turnRight(int speed){
    leftMotor.forward(speed);
    rightMotor.reverse(speed);
}


