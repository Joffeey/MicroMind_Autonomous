#include <Arduino.h>
#include <NRF52_Radio_library.h>

NRF52_Radio radio;



void setup() {
  Serial.begin(9600);
  radio.enable();
  radio.setGroup(1);
  radio.setTransmitPower(2);
}

void loop() {
  if(radio.dataReady() > 0){
    FrameBuffer* receivedBuffer = radio.recv();
    int receivedData;

    if(receivedBuffer->length == sizeof(recievedData)){
      memcpy(&receivedData, receivedBuffer->payload, receivedBuffer->length);

      Serial.print("Received data: ");
      Serial.println(receivedData);
    }
    delete receivedBuffer;
  }

}
