// StateMachine.cpp

#include <StateMachine.h>

// IR sensor pins
const int frontSensPin = 1;
const int leftSensPin = 1;
const int leftAngledPin = 1;
const int rightSensPin = 1;
const int rightAngledPin = 1;
const int threshold = 200;

StateMachine::StateMachine(Micromind& mind) :
    frontSens(frontSensPin), leftSens(leftSensPin), leftAngledSens(leftAngledPin),
    rightSens(rightSensPin), rightAngledSens(rightAngledPin), micromind(mind)
{
    currentState = STOP;
}

void StateMachine::updateState()
{
    if (frontSens.read() > 200)
    {
        currentState = STOP;
    }
    if(frontSens.read() < 200){
        currentState = FORWARD;
    }
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
    }
}