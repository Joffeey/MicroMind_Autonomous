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
    FrameBuffer* recievedBuffer = radio.recv();
    int recievedData;

    if(recievedBuffer->length == sizeof(recievedData)){
      memcpy(&recievedData, recievedBuffer->payload, recievedBuffer->length);

      Serial.print("Recieved data: ");
      Serial.println(recievedData);
    }
    delete recievedBuffer;
  }

}
