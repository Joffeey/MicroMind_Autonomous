// StateMachine.cpp

#include <StateMachine.h>

// IR sensor pins
const int frontSensPin = 1;
const int leftSensPin = 1;
const int leftAngledPin = 1;
const int rightSensPin = 1;
const int rightAngledPin = 1;
// other variables
const int threshold = 200;
int numberOfReadings = 100;
unsigned long startTime, elapsedTime;
int result;
int hallSensorValue; 

StateMachine::StateMachine(Micromind& mind) :
    frontSens(frontSensPin), leftSens(leftSensPin), leftAngledSens(leftAngledPin),
    rightSens(rightSensPin), rightAngledSens(rightAngledPin), micromind(mind){
    currentState = STOP;        // Initially set to stop.
    pinMode(mind.readHallSensor(), INPUT); 
}

void StateMachine::updateState(){
    result = 0;
    startTime = millis();
    elapsedTime = 0;

    for(int i = 0; i < numberOfReadings; i++){
        result += frontSens.read();
    }
    result = result / numberOfReadings;
    elapsedTime = (millis() - startTime);
    Serial.print("The sensor value average after ");
    Serial.print(numberOfReadings);
    Serial.print(" readings is: ");
    Serial.print(result);
    Serial.print(" and it took: ");
    Serial.print(elapsedTime);
    Serial.print(" milliseconds to generate");
    Serial.println(". Currentstate is now: " + String(currentState));

    // Logic for when to stop
    if (result > 500 && result <= 650){
        currentState = STOP;   
    }
    // Logic for when to drive forwards
    if(result <= 500){
        currentState = FORWARD;
    }
    // Logic for when to reverse
    if(result > 650 && result <= 950){
        currentState = REVERSE;
    }

    // Logic for when to turnAround
    if(result > 950){
        currentState = TURN_AROUND;
    }
    // Implement other logic as we get more sensors in place..
}

void StateMachine::executeStateLogic(){
    switch(currentState)
    {
        case FORWARD:
            micromind.forward(512);
            break;
        case STOP:
            micromind.stop();
            break;
        case REVERSE:
            micromind.reverse(512);
            break;
        case TURN_LEFT:
            micromind.turnLeft(255);
            break;
        case TURN_RIGHT:
            micromind.turnRight(255);
            break;
        case TURN_AROUND:
            micromind.turnAround(255);
            break;
    }

    if(currentState == TURN_LEFT || currentState ==TURN_RIGHT ||currentState == TURN_AROUND){
        hallSensorValue = micromind.readHallSensor(); 
    }

}

int StateMachine::getResult() {
    return result;
}