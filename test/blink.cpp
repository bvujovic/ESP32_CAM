#include <Arduino.h>

// const int pinLed = 33; // ugradjeni mali LED: false ON, true OFF
// const int pinLed = 4;  // ugradjeni jak  LED: false OFF, true ON
const int pinLed = 33;

void setup()
{
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  delay(100);
  Serial.println("tu");
  digitalWrite(pinLed, false);
  delay(1500);
  Serial.println("ru");
  digitalWrite(pinLed, true);
  delay(1500);
}