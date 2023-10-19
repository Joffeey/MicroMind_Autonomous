#include <Arduino.h>
#include <NRF52_Radio_library.h>
#include <Adafruit_Microbit.h>

NRF52_Radio radio;
Adafruit_Microbit_Matrix display;

enum Mode {MANUAL, AUTO};
Mode currentMode = MANUAL;

// Button
const int buttonAPin = PIN_BUTTON_A;
const int debounceDelay = 100;
unsigned long lastButtonPress = 0;
bool buttonPressed = false;


bool sendAccelDataFlag = false;

bool waitForConfirmation(){
  unsigned long startTime = millis();
  FrameBuffer* confirmationBuffer = radio.recv();
  while(millis() - startTime < 2000){
    confirmationBuffer = radio.recv();
    if(confirmationBuffer != nullptr && confirmationBuffer->payload[0] == 'C'){
      delete confirmationBuffer;
      return true;  // Recieved verification!
    }
      delete confirmationBuffer;
      delay(10);
  }
  return false; // Confirmation not received
}

void sendCurrentMode(){
  FrameBuffer modeBuffer;
  if(currentMode == MANUAL){
    modeBuffer.payload[0] = 'M';
    radio.send(&modeBuffer);
    Serial.println("Sent 'M'");
  }
  else if(currentMode == AUTO){
    modeBuffer.payload[0] = 'A';
    radio.send(&modeBuffer);
    Serial.println("Sent 'A'");
  }
  else{
    Serial.println("Not valid current mode!");
  }
}

void sendAccelData(){
  // Code to send over accelerometer data while the state continues to be manual up until the point where the user decides to switch to auto mode!
  FrameBuffer accelBuffer;
  accelBuffer.payload[0] = 'F';
  radio.send(&accelBuffer);
  delay(200);
  accelBuffer.payload[0] = 'B';
  radio.send(&accelBuffer);
  delay(200);
  accelBuffer.payload[0] = 'F';
  radio.send(&accelBuffer);
  delay(200);
  accelBuffer.payload[0] = 'B';
  radio.send(&accelBuffer);
  delay(200);
}

void changeMode(){
  display.clear();
  if(currentMode == AUTO){
    currentMode = MANUAL;
    sendCurrentMode();
    Serial.println("Requesting mode change to MANUAL, transmitting 'M' and awaiting verification!");
    if(waitForConfirmation()){
        Serial.println("Mode change confirmed, Switching to MANUAL mode.");
        sendAccelDataFlag = true;
        display.drawLine(0, 0, 0, 4, LED_ON);
        display.drawLine(4, 0, 4, 4, LED_ON);
        display.drawLine(1, 1, 2, 2, LED_ON);
        display.drawLine(3,1, 3, 1, LED_ON);
        sendAccelData();
    }
  }
  else{
    currentMode = AUTO;
    sendCurrentMode();
    Serial.println("Requesting mode change to AUTO, transmitting 'A' and awaiting verification!");
    if(waitForConfirmation()){
      Serial.println("Mode change confirmed, switching to AUTO mode.");
      sendAccelDataFlag = false;
      // Draw 'A' in the matrix -- not needed but for fun
      display.drawLine(0, 0, 0, 4, LED_ON);
      display.drawLine(4, 0, 4, 4, LED_ON);
      display.drawLine(1, 0, 3, 0, LED_ON);
      display.drawLine(1, 2, 3, 2, LED_ON);
    }
  }
}

void checkButton() {
  static unsigned long lastButtonPress = 0;
  unsigned long currentTime = millis();
  
  if (digitalRead(buttonAPin) == LOW) {
    if (currentTime - lastButtonPress >= debounceDelay) {
      // Button pressed, debounce delay passed
      lastButtonPress = currentTime;
      changeMode(); // Toggle mode
    }
    return;
  }
  return;
}


void setup() {
  Serial.begin(115200);
  radio.enable();
  radio.setGroup(2);
  radio.setTransmitPower(2);
  display.begin();
  pinMode(buttonAPin, INPUT_PULLDOWN);
  // Initially sat to M, displaying 'M' in the matrix.
  display.drawLine(0, 0, 0, 4, LED_ON);
  display.drawLine(4, 0, 4, 4, LED_ON);
  display.drawLine(1, 1, 2, 2, LED_ON);
  display.drawLine(3,1, 3, 1, LED_ON);
}

void loop() {
  // Check the button press
  unsigned long currentTime = millis();
  if (digitalRead(buttonAPin) == LOW) {
    if (!buttonPressed && (currentTime - lastButtonPress >= debounceDelay)) {
      // Button pressed, debounce delay passed, and not previously pressed
      buttonPressed = true;
      lastButtonPress = currentTime;

      // Toggle the mode
      if (currentMode == MANUAL) {
        currentMode = AUTO;
      } else {
        currentMode = MANUAL;
      }

      // Send the new mode
      sendCurrentMode();
      Serial.println("Requesting mode change and transmitting mode: " + String(currentMode));

      // Wait for confirmation
      if (waitForConfirmation()) {
        Serial.println("Mode change confirmed.");
        sendAccelDataFlag = (currentMode == MANUAL);
        // Update the display if needed
        display.clear();
        if (currentMode == AUTO) {
          display.drawLine(0, 0, 0, 4, LED_ON);
          display.drawLine(4, 0, 4, 4, LED_ON);
          display.drawLine(1, 0, 3, 0, LED_ON);
          display.drawLine(1, 2, 3, 2, LED_ON);
        } else {
          display.drawLine(0, 0, 0, 4, LED_ON);
          display.drawLine(4, 0, 4, 4, LED_ON);
          display.drawLine(1, 1, 2, 2, LED_ON);
          display.drawLine(3, 1, 3, 1, LED_ON);
          if (sendAccelDataFlag) {
            sendAccelData();
          }
        }
      }
    }
  } else {
    buttonPressed = false;
  }
}
