// Main.cpp
#include <Arduino.h>
#include <NRF52_Radio_library.h>
#include <Micromind.h>
#include <StateMachine.h>
#include <Adafruit_Microbit.h>


NRF52_Radio radio = NRF52_Radio();

FrameBuffer *myConfSendConf;
FrameBuffer* recievedData;

const int radioFrequency = 7; // 2.407GHz Radio
const int protocol = 14;
const int radioVersion = 12;
const uint8_t dataGroup = 1;
bool got_data = false;
char driveCommand;

  // Motor1 pins
  const int motor11 = 8;
  const int motor12 = 12;
  // Motor2 pins
  const int motor21 = 0;
  const int motor22 = 16;

  const int hallSensorPin = 5; //pin position
  // IR sensor pins
  const int IRSens1Pin= 3;
  const int IRSens2Pin = 4;

  // Result of sensor reading
  int myResult;
  int frontRes;
  int rearRes;
  
  bool handleAccelData = false;
  bool isReversing = false;


  Micromind micromind(motor11, motor12, motor21, motor22, hallSensorPin);
  IRSensor IRSens1(IRSens1Pin);
  IRSensor IRSens2(IRSens2Pin);
  StateMachine micromindState(micromind);
  enum Mode { MANUAL, AUTO, STARTUP};
  Mode currentMode = STARTUP;

  Adafruit_Microbit_Matrix display;

  void changeMode(char modeBuffer){
  if(modeBuffer == 'M' && currentMode != MANUAL){
    currentMode = MANUAL;
    display.clear();
    Serial.println("Sucessfully switched to manual mode -> Sending confirmation");
    radio.send(myConfSendConf);
    handleAccelData = true;
    got_data = false;
    display.drawLine(0, 0, 0, 4, LED_ON);
    display.drawLine(4, 0, 4, 4, LED_ON);
    display.drawLine(1, 1, 2, 2, LED_ON);
    display.drawLine(3,1, 3, 1, LED_ON);
  }
  else if(modeBuffer == 'A' && currentMode != AUTO){
    Serial.println("Sucessfully switched to Auto mode -> Sending confirmation");
    currentMode = AUTO;
    display.clear();
    radio.send(myConfSendConf);
    handleAccelData = false;
    got_data = false;
    display.drawLine(0, 0, 0, 4, LED_ON);
    display.drawLine(4, 0, 4, 4, LED_ON);
    display.drawLine(1, 0, 3, 0, LED_ON);
    display.drawLine(1, 2, 3, 2, LED_ON);
  }
  else{
    Serial.println("No switch initiated, currently in the mode desired mode.");
  }
}

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
  else if (direction == 'S'){
    Serial.println("Recieved 'S' stopping!");
    micromind.stop();
  }
  else{
    Serial.println("Received unknown command!");
  }
}

void checkRadio() {
  if(radio.dataReady() > 0){
    Serial.println("There's something ready to be recieved!");
    recievedData = radio.recv();
    if(recievedData->payload[0] == 'A'){
      Serial.println("Data recieved is verified to be 'A'");
      changeMode('A');      
    }
    else if(recievedData->payload[0] == 'M'){
      Serial.println("Data recieved is verified to be 'M'");
      changeMode('M');
    }
    else if(handleAccelData && recievedData->payload[0] == 'B' || recievedData->payload[0] == 'F' || recievedData->payload[0] == 'R' || recievedData->payload[0] == 'L'){
      Serial.print("Data recieved is verified to be accelerometer data");
      if(recievedData->payload[0] == 'B'){
        drive('B');
      }
      if(recievedData->payload[0] == 'F'){
        drive('F');
      }
      if(recievedData->payload[0] == 'R'){
        drive('R');
      }
      if(recievedData->payload[0] == 'L'){
        drive('L');
      }
      if(recievedData->payload[0] == 'S'){
        drive('S');
      }
    }
    }
    else{
      Serial.println("There's no data ready to be recieved..");
    }
  }


void setup() 
{
  Serial.begin(115200);
  myConfSendConf = new FrameBuffer();
  myConfSendConf->length = 4; // 3 (fixed) + MSGSIZE
  myConfSendConf->version = radioVersion;
  myConfSendConf->group = dataGroup;
  myConfSendConf->protocol = protocol;
  myConfSendConf->payload[0] = 'C';
  
  radio.enable();
  radio.setFrequencyBand(radioFrequency);
  radio.setGroup(dataGroup);
  display.begin();
  // Initially set to S, displaying 'S' in the matrix.
  display.drawLine(0, 0, 4, 0, LED_ON);
  display.drawLine(0, 2, 4, 2, LED_ON);
  display.drawLine(0, 4, 4, 4, LED_ON);
  display.drawPixel(0, 1, LED_ON);
  display.drawPixel(4, 3, LED_ON);
}

void loop() 
{
 checkRadio();
 if(currentMode == AUTO){
  micromindState.updateState();
  micromindState.executeStateLogic();
 }
 if(currentMode == MANUAL && handleAccelData){
  
 }
 Serial.println("Current state is " + String(currentMode));
 delay(10);
}
