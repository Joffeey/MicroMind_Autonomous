#include <Arduino.h>

void setup() {
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);

  pinMode(PIN_A0, OUTPUT);
  pinMode(16, OUTPUT);

  Serial.begin(115200);
}

void loop() {
    analogWrite(8, 0);
    analogWrite(12, 255);
    analogWrite(0, 0);
    analogWrite(16, 255);
    delay(5000);

    analogWrite(8, 255);
    analogWrite(12, 0);
    analogWrite(0, 255);
    analogWrite(16, 0);
    delay(5000);
}