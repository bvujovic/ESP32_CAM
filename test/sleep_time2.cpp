// za doradu pogledati: https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino

#include <Arduino.h>

#include "WiFiServerBasics.h"
#include "EasyINI.h"
EasyINI *ei;

#include "TimeESP3.h"
RTC_DATA_ATTR TimeESP3 timer;
struct tm t;

RTC_DATA_ATTR int cnt = 0;

#define uS_TO_S 1000000
const int pinLed = 33;

void statusLedON(bool on) { digitalWrite(pinLed, !on); }

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Counter: " + String(cnt));
    pinMode(pinLed, OUTPUT);
    statusLedON(true);

    if (!timer.isInitialized())
    {
        ei = new EasyINI("/times.log");
        ei->open(FMOD_READ);
        timer.setCoefError(ei->getInt("coefError"));
        Serial.println("loaded coefError: " + String(timer.getCoefError()));
        ei->close();
    }
    if (cnt % 5 == 0)
    {
        ConnectToWiFi();
        timer.getNetTime(t);
    }
    else
        timer.getLocalTime(t);

    Serial.println(String(t.tm_min) + ":" + String(t.tm_sec));

    statusLedON(false);

    cnt++;
    Serial.println("sleep...");
    esp_sleep_enable_timer_wakeup(uS_TO_S * 60 * 1);
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}