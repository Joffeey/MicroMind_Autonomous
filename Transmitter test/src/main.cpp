#include <Arduino.h>
#include <NRF52_Radio_library.h>
#include <Adafruit_Microbit.h>

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
const uint8_t confirmationGroup = 2;
bool sendAccelDataFlag = false;


enum Mode {MANUAL, AUTO};
Mode currentMode = MANUAL;

// Button
const int buttonAPin = PIN_BUTTON_A;
const int debounceDelay = 100;



bool waitForConfirmation(){
  unsigned long startTime = millis();
  while(millis() - startTime < 2000){
    radio.setGroup(confirmationGroup);
    recievedData = radio.recv();
    if(recievedData->group == 2){
      if(recievedData->payload[0] == 'C'){
        Serial.println("Recieved confirmation!");
        delete recievedData;
        radio.setGroup(dataGroup);
        return true;
        
      }
      Serial.println("Data recieved belongs to correct protocol, but it's not 'C'!");
      Serial.println("Recieved confirmation contains: " + String(recievedData->payload[0]));
      delete recievedData;
      radio.setGroup(dataGroup);
      return false;  // Recieved verification!
    }
  }
  Serial.println("No confirmation recieved!");
  Serial.println("The recieved data with correct protocol contains: " + String(recievedData->payload[0]));
  radio.setGroup(dataGroup);
  return false; // Confirmation not received
}

void sendCurrentMode(){
  if(currentMode == MANUAL){
    myModeSendMode->payload[0] = 'M';
    radio.send(myModeSendMode);
    Serial.println("Sent 'M'");
  }
  else if(currentMode == AUTO){
    myModeSendMode->payload[0] = 'A';
    radio.send(myModeSendMode);
    Serial.println("Sent 'A'");
  }
  else{
    Serial.println("Not valid current mode!");
  }
}

void sendOppositeMode(){
  if(currentMode == MANUAL){
    myModeSendMode->payload[0] = 'A';
    radio.send(myModeSendMode);
    Serial.println("Sent 'A'");
  }
  else if(currentMode == AUTO){
    myModeSendMode->payload[0] = 'M';
    radio.send(myModeSendMode);
    Serial.println("Sent 'M'");
  }
  else{
    Serial.println("Not valid current mode!");
  }
}

void sendAccelData(){
  // Code to send over accelerometer data while the state continues to be manual up until the point where the user decides to switch to auto mode!

  myAccelSendAccel->payload[0] = 'F';
  radio.send(myAccelSendAccel);
  delay(200);
  myAccelSendAccel->payload[0] = 'B';
  radio.send(myAccelSendAccel);
  delay(200);
  myAccelSendAccel->payload[0] = 'F';
  radio.send(myAccelSendAccel);
  delay(200);
  myAccelSendAccel->payload[0] = 'B';
  radio.send(myAccelSendAccel);
  delay(200);
}

void changeMode(){
  display.clear();
  if(currentMode == AUTO){
    sendOppositeMode();
    Serial.println("Requesting mode change to MANUAL, transmitting 'M' and awaiting verification!");
      if(radio.dataReady() > 0){
        Serial.println("There's data ready for us!");
        if(waitForConfirmation()){
        Serial.println("Mode change confirmed, Switching to MANUAL mode.");
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
  else if(currentMode == MANUAL){
    sendOppositeMode();
    Serial.println("Requesting mode change to AUTO, transmitting 'A' and awaiting verification!");
      if(radio.dataReady() > 0){
        Serial.println("There's data ready for us!");
        if(waitForConfirmation()){
        Serial.println("Mode change confirmed, Switching to AUTO mode.");
        currentMode = AUTO;
        sendAccelDataFlag = false;
       // Draw 'A' in the matrix -- not needed but for fun
       display.drawLine(0, 0, 0, 4, LED_ON);
       display.drawLine(4, 0, 4, 4, LED_ON);
       display.drawLine(1, 0, 3, 0, LED_ON);
       display.drawLine(1, 2, 3, 2, LED_ON);
     }
   }
   else{
    Serial.println("There's no data for us -> Returning.");
    if(digitalRead(buttonAPin) == LOW){
      Serial.print("Im printing out this inside the changemode because button is still low");
    }
    return;
   }
 }
}

void checkButton() {
  static unsigned long lastButtonPress = 0;
  static bool buttonPressed = false;
  unsigned long currentTime = millis();
  
  if (digitalRead(buttonAPin) == LOW) {
    if(!buttonPressed){
      Serial.println("Button pressed");
      lastButtonPress = currentTime;
      buttonPressed = true;
    }
    if (currentTime - lastButtonPress >= debounceDelay || currentTime < lastButtonPress) {
      if(buttonPressed){
        Serial.println("Current time: " + String(currentTime) + ", Last button press: " + String(lastButtonPress));
        changeMode();
        buttonPressed = false;
      }
    }
  }
  else{
    buttonPressed = false;
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
  pinMode(buttonAPin, INPUT_PULLUP);

  // Initially sat to M, displaying 'M' in the matrix.
  display.drawLine(0, 0, 0, 4, LED_ON);
  display.drawLine(4, 0, 4, 4, LED_ON);
  display.drawLine(1, 1, 2, 2, LED_ON);
  display.drawLine(3,1, 3, 1, LED_ON);
}

void loop() {
  checkButton();
  if(sendAccelDataFlag == true && currentMode == MANUAL){
    sendAccelData();
  }
}
