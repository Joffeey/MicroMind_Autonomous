// Micromind.cpp
#include <Micromind.h>

// leftMotor = brun 218
// rightMotor = hvit    233
const int forwardScale = 65;
const int reverseScale = 1;
const long turnScale = 5000;
const long turnAroundScale = 10000;

Micromind::Micromind(int leftPin1, int leftPin2, int rightPin1, int rightPin2) : leftMotor(leftPin1, leftPin2), rightMotor(rightPin1, rightPin2){}
unsigned long turnStartTime = 0;

void Micromind::forward(int speed){
    leftMotor.forward(speed + forwardScale);
    rightMotor.forward(speed);
}

void Micromind::reverse(int speed){
    leftMotor.reverse(speed);
    rightMotor.reverse(speed - reverseScale);
}

void Micromind::stop(){
    leftMotor.stop();
    rightMotor.stop();
}


bool Micromind::turnLeft(int speed){
    if(turnStartTime == 0){
        turnStartTime = millis();
        leftMotor.reverse(speed);
        rightMotor.forward(speed);
    }

    if(millis() - turnStartTime >= turnScale){
        leftMotor.stop();
        rightMotor.stop();
        turnStartTime = 0;
        return true;
    }
    return false;
}

bool Micromind::turnRight(int speed){
    if(turnStartTime == 0){
        turnStartTime = millis();
        leftMotor.forward(speed);
        rightMotor.reverse(speed);
    }

    if(millis() - turnStartTime >= turnScale){
        leftMotor.stop();
        rightMotor.stop();
        turnStartTime = 0;
        return true;
    }
    return false;
}

bool Micromind::turnAround(int speed){
    Serial.print("Current value of turnStartTime: ");
    Serial.println(turnStartTime);
    if(turnStartTime == 0){
        turnStartTime = millis();
        leftMotor.reverse(speed);
        rightMotor.forward(speed);
    }

    if(millis() - turnStartTime >= turnAroundScale){
        leftMotor.stop();
        rightMotor.stop();
        turnStartTime = 0;
        return true;
    }
    return false;
}