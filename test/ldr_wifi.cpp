#include <Arduino.h>

#include "DataLogger.h"
const int cntMsgs = 5;
DataLogger<String, cntMsgs> *msgs;

const int pinLdr = 13;

#include "WiFiServerBasics.h"

#include "time.h"
#include "lwip/apps/sntp.h"
struct tm ti; // Tekuce/trenutno vreme.
RTC_DATA_ATTR int cntNoInitTime = 2;
tm tiInit; // Vreme poslednjeg uzimanja tacnog vremena sa interneta.
// za doradu pogledati: https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino

void getTime(int adjustSec = 0)
{
    time_t now;
    time(&now);
    Serial.println(now);
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
    // while (1)
    // {
    //     getTime();
    //     if (ti.tm_year >= 2020)
    //         break;
    //     if (++i < max)
    //         delay(2000);
    // }

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

    if (cntNoInitTime == 4)
        Serial.println(WiFi.disconnect());
    pinMode(pinLdr, INPUT);
    int valLdr = analogRead(pinLdr);
    Serial.println("LDR: " + String(valLdr));

    // msgs = new DataLogger<String, cntMsgs>("/times.log", DataLogger<String, cntMsgs>::stringToFile);
    // Serial.println(msgs->readFromFile());
    // Serial.println("***");
    // while (true)
    //     delay(100);

    getTime();
    Serial.println("cnt wake up: " + String(cntNoInitTime));
    Serial.println("wifi status: " + String(WiFi.status()));
    Serial.println("wifi status: " + String(WiFi.getMode()));
    // if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER)
    if (cntNoInitTime <= 0)
    {
        cntNoInitTime = 6;
        ConnectToWiFi();
        initTime();

        //* LDR na pinu 13 radi dobro dok se ESP ne konektuje ne WiFi.
        //* Ovo je neuspeo pokusaj da se ukine taj problem.
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        btStop();
        delay(100);
        Serial.println("LDR2: " + String(analogRead(pinLdr)));
        // esp_wifi_disconnect(); // break connection to AP
        // esp_wifi_stop();       // shut down the wifi radio
        // esp_wifi_deinit();     // release wifi resources
    }
    else
        cntNoInitTime--;

    // getTime();
    // ulong ms = millis();
    // // Serial.println(ti.tm_sec);
    // msgs->add(String(ti.tm_sec));
    // initTime();
    // Serial.println(millis() - ms);
    // msgs->add(String(millis() - ms));
    // Serial.println(ti.tm_sec);
    // msgs->add(String(ti.tm_sec));
    // msgs->saveToFile();
    // Serial.println();
    statusLedON(false);

    Serial.println("sleep...");
    esp_sleep_enable_timer_wakeup(uS_TO_S * 10 * 1);
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}