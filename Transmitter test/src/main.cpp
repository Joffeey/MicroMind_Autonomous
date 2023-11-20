#include <Arduino.h>
#include <NRF52_Radio_library.h>
#include <Adafruit_Microbit.h>
#include <Adafruit_LSM303_Accel.h>


Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

NRF52_Radio radio = NRF52_Radio();
Adafruit_Microbit_Matrix display;

FrameBuffer *myModeSendMode;
FrameBuffer *myAccelSendAccel;
FrameBuffer* recievedData;
const long interval = 1000; // 1s.
const int MSGSIZE = 1; // max 32 bit.
u_char msg[MSGSIZE]; // since global, it will init all to zero's
const int radioFrequency = 7; // 2.407GHz Radio
const int protocol = 14;
const int radioVersion = 12;
const uint8_t dataGroup = 1;
bool sendAccelDataFlag = false;


enum Mode {MANUAL, AUTO, STARTUP};
Mode currentMode = STARTUP;

// Button
const int debounceDelay = 100;



bool waitForConfirmation(){
  unsigned long startTime = millis();
  while(millis() - startTime < 2000){
    recievedData = radio.recv();
    if(recievedData->group == 1 && recievedData->protocol == 14){
      if(recievedData->payload[0] == 'C'){
        Serial.println("Recieved confirmation!");
        delete recievedData;
        return true;
        
      }
      Serial.println("Data recieved belongs to correct protocol and group, but it's not 'C'!");
      Serial.println("Recieved confirmation contains: " + String(recievedData->payload[0]));
      delete recievedData;
      return false;
    }
  }
  Serial.println("No confirmation recieved!");
  Serial.println("The recieved data with correct protocol contains: " + String(recievedData->payload[0]));
  return false; // Confirmation not received
}

void sendMode(char mode){
  if(mode == 'M'){
    myModeSendMode->payload[0] = 'M';
    radio.send(myModeSendMode);
    Serial.println("Sent 'M'");
  }
  else if(mode == 'A'){
    myModeSendMode->payload[0] = 'A';
    radio.send(myModeSendMode);
    Serial.println("Sent 'A'");
  }
  else{
    Serial.println("Not valid current mode!");
  }
}

void sendAccelData(){
  // Code to send over accelerometer data while the state continues to be manual up until the point where the user decides to switch to auto mode!
  sensors_event_t event;
  accel.getEvent(&event);
  if(event.acceleration.y >= 8){
      myAccelSendAccel->payload[0] = 'F';
      radio.send(myAccelSendAccel);
  }
  if(event.acceleration.y <= -8){
    myAccelSendAccel->payload[0] = 'B';
    radio.send(myAccelSendAccel);
  }
  if(event.acceleration.x >= 8){
    myAccelSendAccel->payload[0] = 'R';
    radio.send(myAccelSendAccel);
  }
  if(event.acceleration.x <= -8){
    myAccelSendAccel->payload[0] = 'L';
    radio.send(myAccelSendAccel);
  }
  if(event.acceleration.z <= -9){
    myAccelSendAccel->payload[0] = 'S';
    radio.send(myAccelSendAccel);
  }
}

void changeMode(char mode){
  display.clear();
  if(mode == 'M' || mode == 'A'){
    sendMode(mode);
    Serial.println("Requesting mode change, transmitting and awaiting verification!");
      if(radio.dataReady() > 0){
        Serial.println("There's data ready for us!");
        if(waitForConfirmation()){
        Serial.println("Mode change confirmed, Switching mode.");
        if(mode == 'A'){
          currentMode = AUTO;
          sendAccelDataFlag = false;
          display.drawLine(0, 0, 0, 4, LED_ON);
          display.drawLine(4, 0, 4, 4, LED_ON);
          display.drawLine(1, 0, 3, 0, LED_ON);
          display.drawLine(1, 2, 3, 2, LED_ON);
        }
        else if(mode == 'M'){
          currentMode = MANUAL;
          sendAccelDataFlag = true;
          display.drawLine(0, 0, 0, 4, LED_ON);
          display.drawLine(4, 0, 4, 4, LED_ON);
          display.drawLine(1, 1, 2, 2, LED_ON);
          display.drawLine(3,1, 3, 1, LED_ON);
          sendAccelData();
        }
        }
        }
        else{
          Serial.println("There's no data for us -> Returning.");
          if(currentMode == MANUAL){
            display.drawLine(0, 0, 0, 4, LED_ON);
            display.drawLine(4, 0, 4, 4, LED_ON);
            display.drawLine(1, 1, 2, 2, LED_ON);
            display.drawLine(3,1, 3, 1, LED_ON);
            }
         else if(currentMode == AUTO){
          display.drawLine(0, 0, 0, 4, LED_ON);
          display.drawLine(4, 0, 4, 4, LED_ON);
          display.drawLine(1, 0, 3, 0, LED_ON);
          display.drawLine(1, 2, 3, 2, LED_ON);
          }
         else{
          display.drawLine(0, 0, 4, 0, LED_ON);
          display.drawLine(0, 2, 4, 2, LED_ON);
          display.drawLine(0, 4, 4, 4, LED_ON);
          display.drawPixel(0, 1, LED_ON);
          display.drawPixel(4, 3, LED_ON);
          }   
        }
  }
}


void checkButtonA() {
  if (digitalRead(PIN_BUTTON_A) == LOW) {
      if(currentMode != MANUAL){
        changeMode('M');
        Serial.println("Button A pressed and trying to swap mode!");
      }
      else{
        Serial.println("Already in Manual mode -> Returning!");
      }
    }
}

void checkButtonB() {
  if (digitalRead(PIN_BUTTON_B) == LOW) {
      if(currentMode != AUTO){
        changeMode('A');
        Serial.println("Button B pressed and trying to swap mode!");
      }
      else{
        Serial.println("Already in Auto mode -> Returning!");
      }
    }
}


void setup() {
  Serial.begin(115200);
  myModeSendMode = new FrameBuffer();
  myModeSendMode->length = 4; // 3 (fixed) + MSGSIZE
  myModeSendMode->version = radioVersion;
  myModeSendMode->group = dataGroup;
  myModeSendMode->protocol = protocol;
  myModeSendMode->payload[0] = 'M';

  myAccelSendAccel = new FrameBuffer();
  myAccelSendAccel->length = 4;
  myAccelSendAccel->version = radioVersion;
  myAccelSendAccel->group = dataGroup;
  myAccelSendAccel->protocol = protocol;

  radio.enable();
  radio.setFrequencyBand(radioFrequency);
  radio.setGroup(dataGroup);
  display.begin();
  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  pinMode(PIN_BUTTON_B, INPUT_PULLUP);

  // Initially set to S, displaying 'S' in the matrix.
  display.drawLine(0, 0, 4, 0, LED_ON);
  display.drawLine(0, 2, 4, 2, LED_ON);
  display.drawLine(0, 4, 4, 4, LED_ON);
  display.drawPixel(0, 1, LED_ON);
  display.drawPixel(4, 3, LED_ON);

    /* Initialise the sensor */
  accel.begin();
  accel.setMode(LSM303_MODE_NORMAL);
}

void loop() {
  checkButtonA();
  checkButtonB();
  if(sendAccelDataFlag == true && currentMode == MANUAL){
    sendAccelData();
  }
}
