#include <Arduino.h>

const int fan_1 = 4;

void setup() {
  pinMode(fan_1, OUTPUT);
  digitalWrite(fan_1, HIGH);
}

void loop() {
  delay(5000);
  digitalWrite(fan_1, LOW);
  delay(5000);
  digitalWrite(fan_1, HIGH);
}