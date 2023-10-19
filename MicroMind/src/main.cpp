// Main.cpp
#include <Arduino.h>
#include <NRF52_Radio_library.h>
#include <Micromind.h>
#include <StateMachine.h>
#include <Adafruit_Microbit.h>


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
  uint8_t receiveDir = 0;

  bool handleAccelData = false;


  Micromind micromind(motor11, motor12, motor21, motor22);
  IRSensor IRSens1(IRSens1Pin);
  StateMachine micromindState(micromind);
  enum Mode { MANUAL, AUTO};
  Mode currentMode = MANUAL;

  Adafruit_Microbit_Matrix display;

void drive(char direction){
  if(direction == 'F'){
    Serial.println("Recieved 'F' driving forwards!");
    micromind.forward(255);
  }
  else if(direction == 'B'){
    Serial.println("Recieved 'B' reversing!");
    micromind.reverse(255);
  }
  else if(direction == 'L'){
    Serial.println("Recieved 'L' turning left!");
    micromind.turnLeft(255);
  }
  else if(direction == 'R'){
    Serial.println("Recieved 'R' turning right!");
    micromind.turnRight(255);
  }
  else{
    Serial.println("Recieved unvalid command -> buhu");
  }
}

void changeMode(char modeBuffer){
  FrameBuffer confirmationBuffer;
  confirmationBuffer.payload[0] = 'C';
  radio.send(&confirmationBuffer);
  if(modeBuffer == 'M'){
    currentMode = MANUAL;
    Serial.println("Sucessfully switched to manual mode -> Sending confirmation");
    radio.send(&confirmationBuffer);
    handleAccelData = true;
  }
  else{
    Serial.println("Sucessfully switched to Auto mode -> Sending confirmation");
    currentMode = AUTO;
    radio.send(&confirmationBuffer);
    handleAccelData = false;
  }
}


void setup() 
{
  Serial.begin(115200);
  radio.enable();
  radio.setGroup(2);
  radio.setTransmitPower(2);
}

void loop() 
{
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();

  FrameBuffer* commandBuffer = radio.recv();
  char command = commandBuffer->payload[0];
  if(command != NULL){
    if(command == 'A'  && currentMode == MANUAL || command == 'M' && currentMode == AUTO){
     changeMode(command);
  }
    else if(currentMode == MANUAL && handleAccelData == true){
    drive(command);
  }
    else
  {
    if(currentTime - lastPrintTime > 2000){
      Serial.println("Recieved an unknown command: " + String(command));
      Serial.println(currentMode == MANUAL ? "MANUAL" : "AUTO");
      lastPrintTime = currentTime;
    }
  }
  delete commandBuffer;
  }
  if (currentMode == AUTO){
    micromindState.updateState();
    micromindState.executeStateLogic();
  }
}
