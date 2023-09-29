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

StateMachine::StateMachine(Micromind& mind) :
    frontSens(frontSensPin), leftSens(leftSensPin), leftAngledSens(leftAngledPin),
    rightSens(rightSensPin), rightAngledSens(rightAngledPin), micromind(mind)
{
    currentState = STOP;
}

void StateMachine::updateState()
{
    result = 0;
    startTime = millis();
    elapsedTime = 0;

    for(int i = 0; i < numberOfReadings; i++)
    {
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
    Serial.print(". Currentstate is now: ");
    Serial.println(currentState);
    // Logic for when to stop       jo nærmere jo høyere verdi 600-700 er cirka avstand vi kommer til å stoppe
    if (frontSens.read() > 500 && frontSens.read() <= 650)
    {
        currentState = STOP;   
    }
    // Logic for when to drive forwards
    if(frontSens.read() <= 500){
        currentState = FORWARD;
    }
    // Logic for when to reverse
    if(frontSens.read() > 650){
        currentState = REVERSE;
    }
    // Implement other logic as we get more sensors in place..
}

void StateMachine::executeStateLogic()
{
    switch(currentState)
    {
        case FORWARD:
            micromind.forward(255);
            break;
        case STOP:
            micromind.stop();
            break;
        case REVERSE:
            micromind.reverse(512);
            break;
    }
}