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

  Micromind micromind(motor11, motor12, motor21, motor22);
  IRSensor IRSens1(IRSens1Pin);
  StateMachine micromindState(micromind);
  unsigned long startTime, elapsedTime;
  int numReadings = 1000;
  int result = 0;



void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  result = 0;
  startTime = micros();
  for(int i = 0; i < numReadings; i++){
    result+= IRSens1.read();
    delayMicroseconds(5);
  }
  elapsedTime = (micros() - startTime) / numReadings;
  result = result / numReadings;
  Serial.print("After ");
  Serial.print(numReadings);
  Serial.print(" readings the average is: ");
  Serial.print(result);
  Serial.print(". Time: ");
  Serial.print(elapsedTime);
  Serial.println(" microseconds.");
  delay(1000);

  micromindState.updateState();
  micromindState.executeStateLogic();
}