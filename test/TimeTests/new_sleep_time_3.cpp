#include <Arduino.h>

#include "SleepTimer.h"
RTC_DATA_ATTR SleepTimer timer(5 * 60, 3, -150);
struct tm t;

#include <WiFiServerBasics.h>
WebServer server(80);

void goToSleep()
{
    uint64_t usec = timer.usecToSleep(t);
    esp_sleep_enable_timer_wakeup(usec);
    esp_deep_sleep_start();
}

void setup()
{
    Serial.begin(115200);
    timer.getLocalTime(t);
    Serial.printf("wake %d - %02d:%02d \n", timer.getCountNetTimeCheck(), t.tm_min, t.tm_sec);
    // timer.setWakeEvery(5 * 60);
    // timer.setNetTimeCheck(3);
}

void loop()
{
    if (timer.shouldGetNetTime())
    {
        //T
        // long nowLocal;
        // time(&nowLocal);
        // Serial.printf("nowLocal=%ld\n", nowLocal);
        // Serial.printf("msLocal %ld\n", millis());
        ConnectToWiFi();
        //T Serial.printf("msStart %ld\n", millis());
        timer.getNetTime(t);
        //T Serial.printf("msNet %ld\n", millis());
        Serial.println("CoefError: " + String(timer.getCoefError()));
    }
    else
        timer.getLocalTime(t);

    Serial.println(&t, "%A, %B %d %Y %H:%M:%S");
    timer.waitFor00(t);
    Serial.println(&t, "%A, %B %d %Y %H:%M:%S");
    goToSleep();
}
