// Main.cpp
#include <Arduino.h>
#include <NRF52_Radio_library.h>
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

  // Result of sensor reading
  int myResult;

  // Radio comm.
  NRF52_Radio radio;
  FrameBuffer buffer;

  Micromind micromind(motor11, motor12, motor21, motor22);
  IRSensor IRSens1(IRSens1Pin);
  StateMachine micromindState(micromind);


void setup() 
{
  Serial.begin(9600);
  radio.enable();
  radio.setGroup(1);
  radio.setTransmitPower(2);

}

void loop() 
{
  micromindState.updateState();
  myResult= micromindState.getResult();
  micromindState.executeStateLogic();
  
  buffer.length = sizeof(myResult);
  buffer.version = 1;
  buffer.group = 1;
  buffer.protocol = MICROBIT_RADIO_PROTOCOL_DATAGRAM;
  memcpy(buffer.payload, &myResult, buffer.length);
  radio.send(&buffer);
}