#include <Arduino.h>

#define uS_TO_S_FACTOR 1000000
const int pinLed = 33;

void statusLedON(bool on) { digitalWrite(pinLed, !on); }

void setup()
{
    pinMode(pinLed, OUTPUT);
    statusLedON(true);
    delay(1000);

    esp_sleep_enable_timer_wakeup(5 * uS_TO_S_FACTOR);
    esp_deep_sleep_start();     // 5 sec deep sleep-a. za to vreme ESP trosi 3-4 mA
}

void loop()
{
    delay(100);
}