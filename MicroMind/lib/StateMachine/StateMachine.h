// StateMachine.h

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <IRSensor.h>
#include <Micromind.h>



enum State { FORWARD, STOP, REVERSE, TURN_LEFT, TURN_RIGHT, TURN_AROUND};

class StateMachine {
    private:
    State currentState;
    IRSensor frontSens;
    IRSensor leftSens;
    IRSensor leftAngledSens;
    IRSensor rightSens;
    IRSensor rightAngledSens;
    Micromind& micromind;   // Reference

    public:
    StateMachine(Micromind& Micromind);

    State getState() const { return currentState; }
    void updateState();
    void executeStateLogic();
    int getResult();
};

#endif