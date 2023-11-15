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
const uint8_t sendGroup = 1;
const uint8_t recieveGroup = 1;
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
  const int IRSens1Pin= 1;

  // Result of sensor reading
  int myResult;


  bool handleAccelData = false;


  Micromind micromind(motor11, motor12, motor21, motor22, hallSensorPin);
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
  display.clear();
  if(modeBuffer == 'M'){
    currentMode = MANUAL;
    Serial.println("Sucessfully switched to manual mode -> Sending confirmation");
    radio.send(myConfSendConf);
    handleAccelData = true;
    got_data = false;
    display.drawLine(0, 0, 0, 4, LED_ON);
    display.drawLine(4, 0, 4, 4, LED_ON);
    display.drawLine(1, 1, 2, 2, LED_ON);
    display.drawLine(3,1, 3, 1, LED_ON);
  }
  else{
    Serial.println("Sucessfully switched to Auto mode -> Sending confirmation");
    currentMode = AUTO;
    radio.send(myConfSendConf);
    handleAccelData = false;
    got_data = false;
    display.drawLine(0, 0, 0, 4, LED_ON);
    display.drawLine(4, 0, 4, 4, LED_ON);
    display.drawLine(1, 0, 3, 0, LED_ON);
    display.drawLine(1, 2, 3, 2, LED_ON);
  }
}


void setup() 
{
  Serial.begin(115200);
  myConfSendConf = new FrameBuffer();
  myConfSendConf->length = 4; // 3 (fixed) + MSGSIZE
  myConfSendConf->version = radioVersion;
  myConfSendConf->group = sendGroup;
  myConfSendConf->protocol = protocol;
  myConfSendConf->payload[0] = 'C';
  radio.enable();
  radio.setFrequencyBand(radioFrequency);
  radio.setGroup(sendGroup);
  display.begin();

  // Initially sat to M, displaying 'M' in the matrix.
  display.drawLine(0, 0, 0, 4, LED_ON);
  display.drawLine(4, 0, 4, 4, LED_ON);
  display.drawLine(1, 1, 2, 2, LED_ON);
  display.drawLine(3,1, 3, 1, LED_ON);
}

void loop() 
{
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();

  if((radio.dataReady() > 0) && (got_data != true)){
    recievedData = radio.recv();  // Recieve the ready data.
   if(recievedData->payload[0] == 'A' && currentMode == MANUAL){
      changeMode('A');
      got_data = true;
   }
   if(recievedData->payload[0] == 'M' && currentMode == AUTO){
     changeMode('M');
     got_data = true;
   }
   if(currentMode == MANUAL && handleAccelData == true){
    driveCommand = recievedData->payload[0];
    drive(driveCommand);
   }
  }
    else
  {
    if(currentTime - lastPrintTime > 2000){
      Serial.println("Recieved an unknown command: " + String(recievedData->payload[0]));
      Serial.println(currentMode == MANUAL ? "MANUAL" : "AUTO");
      lastPrintTime = currentTime;
      delete recievedData;
    }
  }
  delete recievedData;
  if (currentMode == AUTO){
    micromindState.updateState();
    micromindState.executeStateLogic();
  }
}

