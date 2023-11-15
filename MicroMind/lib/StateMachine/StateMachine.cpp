// StateMachine.cpp

#include <StateMachine.h>

// IR sensor pins
const int frontSensPin = 3;
const int leftSensPin = 2;
//const int leftAngledPin = 1; //not yet established
const int rightSensPin = 1;  
//const int rightAngledPin = 1; //not yet established


// other variables
const int threshold = 200;
int numberOfReadings = 50;
unsigned long startTime, elapsedTime;
int frontResult;
int rearResult;
bool stopFlag = false; 
int hallSensorValue; 

/* 
//not yet established
StateMachine::StateMachine(Micromind& mind) :
    frontSens(frontSensPin), leftSens(leftSensPin), leftAngledSens(leftAngledPin),
    rightSens(rightSensPin), rightAngledSens(rightAngledPin), micromind(mind){
    currentState = STOP;        // Initially set to stop.
    pinMode(mind.readHallSensor(), INPUT); 
}
*/
StateMachine::StateMachine(Micromind& mind) :
    frontSens(frontSensPin), leftSens(leftSensPin), rightSens(rightSensPin), micromind(mind){
    currentState = STOP;        // Initially set to stop.
    pinMode(mind.readHallSensor(), INPUT); 
}



void StateMachine::updateState(){
    frontResult = 0;
    rearResult = 0; 

   for(int i = 0; i < numberOfReadings; i++){
        frontResult += frontSens.read();
    }

    frontResult = frontResult / numberOfReadings;
    Serial.print("The front sensor value average after ");
    Serial.print(numberOfReadings);
    Serial.print(" readings is: ");
    Serial.print(frontResult);

    for(int j = 0; j < numberOfReadings; j++){
        rearResult += leftSens.read();
    }
    rearResult = rearResult / numberOfReadings;
    Serial.print("The rear sensor value average after ");
    Serial.print(numberOfReadings);
    Serial.print(" readings is: ");
    Serial.print(rearResult);

    // Logic for when to stop
    if (frontResult > 900 && !stopFlag){
        currentState = STOP;
        stopFlag = true;

        if(rearResult <= 15){
            currentState = REVERSE;
        }
        else if(frontResult <= 400){
            currentState = FORWARD;
            stopFlag = false;
        }   
    }
    else if (frontResult < 900 && !stopFlag){
        currentState = FORWARD;
    }

    // Logic for when to turnAround
    /*
    if(frontResult > 950 ){
        currentState = TURN_AROUND;
    }
    */

    // Implement other logic as we get more sensors in place..
}

void StateMachine::executeStateLogic(){
    switch(currentState)
    {
        case FORWARD:
            micromind.forward(200);
            break;
        case STOP:
            micromind.stop();
            break;
        case REVERSE:
            micromind.reverse(200);
            break;
        case TURN_LEFT:
            micromind.turnLeft(255);
            hallSensorValue = micromind.readHallSensor();  // Update hallSensorValue for TURN_LEFT
            break;
        case TURN_RIGHT:
            micromind.turnRight(255);
            hallSensorValue = micromind.readHallSensor();  // Update hallSensorValue for TURN_RIGHT
            break;
        case TURN_AROUND:
            micromind.turnAround(255);
            break;
    }
    if(currentState == TURN_LEFT || currentState ==TURN_RIGHT || currentState == TURN_AROUND){
        hallSensorValue = micromind.readHallSensor(); 
        }


    }

int StateMachine::getResult() {
    return frontResult;
}