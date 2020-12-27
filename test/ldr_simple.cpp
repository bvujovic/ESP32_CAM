#include <Arduino.h>
#include "SD_MMC.h"

const int pinLdr = 13;
const int pinLed = 33;

void setup()
{
    // Serial.begin(115200);
    pinMode(pinLdr, INPUT);
    pinMode(pinLed, OUTPUT);

    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
        ;                               // Serial.println("sd mmc greska");
}

int oldVal = 0;

void loop()
{
    int valLdr = analogRead(pinLdr);
    digitalWrite(pinLed, oldVal == valLdr); // mali LED je upaljen kada su stara i nova vrednost razlicite
    oldVal = valLdr;

    // Serial.println(valLdr);
    delay(500);
}