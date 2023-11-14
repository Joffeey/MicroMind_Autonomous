// Micromind.cpp
#include <Micromind.h>

// leftMotor = brun 218
// rightMotor = hvit    233
const int forwardScale = 65;
const int reverseScale = 1;
const long turnScale = 5000;
const long turnAroundScale = 10000;
const unsigned long defaultHallSensorInterval = 100;



Micromind::Micromind(int leftPin1, int leftPin2, int rightPin1, int rightPin2, int hallSensorPin)
    : leftMotor(leftPin1, leftPin2), rightMotor(rightPin1, rightPin2), hallSensorPin(hallSensorPin) {
}

unsigned long turnStartTime = 0;

int Micromind::readHallSensor() {
    return digitalRead(hallSensorPin);
}


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

    if (millis() -lastHallSensorTime >= hallSensorInterval){
        lastHallSensorTime = millis(); 
        int hallSensorValue = readHallSensor();

        //Apply condition for when it is supposed to take a left turn.
        if(hallSensorValue > hallSensorThreshold) {
            leftMotor.stop();
            rightMotor.stop(); 
            turnStartTime = 0;
            return true; 
        }
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

    if(millis() - lastHallSensorTime >= hallSensorInterval) {
        lastHallSensorTime = millis();
        int hallSensorValue = readHallSensor(); 

        //Apply condition for when it is supposed to take a right turn.
        if(hallSensorValue> hallSensorThreshold) {
            leftMotor.stop();
            rightMotor.stop(); 
            turnStartTime = 0;
            return true;
        }
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

    if(millis() - lastHallSensorTime >= hallSensorInterval){
        lastHallSensorTime = millis(); 
        int hallSensorValue = readHallSensor(); 

        //Apply condition for when it is supposed to turn around.
        if(hallSensorValue > hallSensorThreshold) {
            leftMotor.stop(); 
            rightMotor.stop(); 
            turnStartTime = 0;
            return true; 
        }
    }

    return false;
}
