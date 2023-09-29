// Main.cpp
#include <Arduino.h>
#include <MicrobitV2-HHS.h>
#include <Micromind.h>
#include <StateMachine.h>

// put function declarations here:
  // Motor1 pins
  const int motor11 = 8;
  const int motor12 = 12;
  // Motor2 pins
  const int motor21 = 0;
  const int motor22 = 16;

  // IR sensor pins
  const int IRSens1Pin= 1;

  //Light for the states


  Micromind micromind(motor11, motor12, motor21, motor22);
  IRSensor IRSens1(IRSens1Pin);
  StateMachine micromindState(micromind);

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  micromindState.updateState();
  micromindState.executeStateLogic();
}