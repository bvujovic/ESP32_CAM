#include <Arduino.h>

#include "DataLogger.h"
const int cntMsgs = 5;
DataLogger<String, cntMsgs> *msgs;

const int pinLdr = 13;

#include "WiFiServerBasics.h"

#include "time.h"
#include "lwip/apps/sntp.h"
struct tm ti; // Tekuce/trenutno vreme.
RTC_DATA_ATTR int cntNoInitTime = 0;
tm tiInit; // Vreme poslednjeg uzimanja tacnog vremena sa interneta.
           // za doradu pogledati: https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino

time_t now; // vreme dobijeno sa getTime() - efektivno (long) broj sekundi
void getTime(int adjustSec = 0)
{
    time(&now);
    now += adjustSec;
    localtime_r(&now, &ti);
    ti.tm_year += 1900;
    ti.tm_mon++;
    ti.tm_hour++;
    if (ti.tm_hour == 24)
    {
        ti.tm_hour = 0;
        ti.tm_mday++;
    }
}

void initTime()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    char *srvr = new char[30];
    strcpy(srvr, "rs.pool.ntp.org");
    sntp_setservername(0, srvr);
    sntp_init();
    ti = {0};
    int i = 0;
    const int max = 5;
    while (ti.tm_year < 2020 && ++i < max)
    {
        delay(2000);
        getTime();
    }
    if (i >= max)
        ; // error
}

#define uS_TO_S 1000000
const int pinLed = 33;

void statusLedON(bool on) { digitalWrite(pinLed, !on); }

void setup()
{
    Serial.begin(115200);
    Serial.println();
    pinMode(pinLed, OUTPUT);
    statusLedON(true);

    msgs = new DataLogger<String, cntMsgs>("/times.log", DataLogger<String, cntMsgs>::stringToFile);
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER)
    {
        Serial.println(msgs->readFromFile());
        Serial.println("***");
    }

    getTime();
    long nowSaved = now;
    ulong ms = millis();
    Serial.println("cntNoInitTime: " + String(cntNoInitTime));
    if (cntNoInitTime <= 0)
    {
        cntNoInitTime = 5;
        ConnectToWiFi();
        initTime();
        if (nowSaved > 1000)
        {
            long sec = (millis() - ms + 500) / 1000; // +500 je tu zbog zaokruzivanja umesto odsecanja
            long prediction = nowSaved + sec;
            Serial.println("prediction: " + String(prediction));
            Serial.println("now       : " + String(now));
            msgs->addSave(String(prediction - now));
        }
    }
    else
        cntNoInitTime--;

    statusLedON(false);

    Serial.println("sleep...");
    esp_sleep_enable_timer_wakeup(uS_TO_S * 60 * 1);
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}