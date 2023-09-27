// kitkronikMotorDriver.cpp
#include "kitkronikMotorDriver.h"
#include <Arduino.h>

// 8 , 12

kitkronikMotorDriver::kitkronikMotorDriver(int KMDP1, int KMDP2) : KMDP1(KMDP1), KMDP2(KMDP2){
    pinMode(KMDP1, OUTPUT);
    pinMode(KMDP2, OUTPUT);
    stop();
}


void kitkronikMotorDriver::forward(int speed)
{
    analogWrite(KMDP1, speed);
    analogWrite(KMDP2, 0);
}

void kitkronikMotorDriver::reverse(int speed)
{    
    analogWrite(KMDP1, 0);
    analogWrite(KMDP2, speed);
}

void kitkronikMotorDriver::stop()
{
    analogWrite(KMDP1, 0);
    analogWrite(KMDP2, 0);
}
