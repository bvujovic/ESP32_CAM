#include <Arduino.h>

#include "SleepTimer.h"
RTC_DATA_ATTR SleepTimer timer(0); // -225
struct tm t;
RTC_DATA_ATTR int cnt = 0;

#include <WiFiServerBasics.h>
WebServer server(80);

void goToSleep()
{
    uint64_t usec = timer.usecToSleep(t);
    Serial.print("goToSleep(): us=");
    Serial.println(usec);
    delay(200);
    esp_sleep_enable_timer_wakeup(usec);
    esp_deep_sleep_start();
}

void setup()
{
    timer.getLocalTime(t);
    Serial.begin(115200);
    //B EasyFS::setFileName("/msgs.log");
    //B EasyFS::addf("wake " + String(timer.getCountNetTimeCheck()) + " - " + String(t.tm_min) + ":" + String(t.tm_sec));
    Serial.printf("wake %d - %02d:%02d\n", timer.getCountNetTimeCheck(), t.tm_min, t.tm_sec);
    Serial.println(++cnt);

    const int itv = 60*60;
    // const int itv = 60;
    timer.setWakeEvery(itv);
    // timer.setNetTimeCheck(60 * 60 / itv); // provera vremena ide na svaki sat
    timer.setNetTimeCheck(3);
    if (!timer.isInitialized())
        timer.setCountNetTimeCheck(0);
}

void loop()
{
    //B if (timer.shouldGetNetTime())
    if (cnt > 3)
    {
        ConnectToWiFi();
        timer.getNetTime(t);
        //B EasyFS::addf("CoefError: " + String(timer.getCoefError()));
        Serial.printf("CoefError: %ld\n", timer.getCoefError());
        cnt = 0;
    }
    else
    {
        timer.getLocalTime(t);
    }
    Serial.println(&t, "%A, %B %d %Y %H:%M:%S");
    Serial.printf("%04d-%02d-%02d\n", 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday);
    goToSleep();
}
