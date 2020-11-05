/*
   Merenje brzine mikrokontrolera - brojanje do 10M
   Rezultati:
   ESP32              1.26sec
   ESP32 CAM          1.80sec (4B - long)
   ESP32 CAM          2.10sec (8B - long long)
   ESP8266 (80MHz)    60.3sec
   ESP8266 (160MHz)   32.9sec
   A.Nano             26.4sec
*/

#include <Arduino.h>

long long x = 0;
unsigned long msec;

void setup() {
  Serial.begin(115200);
  Serial.println("x size: " + String(sizeof(x)) + " bytes");
  Serial.println("start...");//  Serial.println(millis());
  msec = millis();
}


void loop() {
  x++;
  if (x > 10000000LL)
  {
    //B Serial.print("end:\t");
    Serial.println(millis() - msec);
    while (true)
      delay(1000);
  }
}
