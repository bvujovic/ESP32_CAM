#include <Arduino.h>

// #include "SleepTimer.h"
// RTC_DATA_ATTR SleepTimer timer(0); // -225
const char *ntpServer = "rs.pool.ntp.org";
struct tm t;
time_t now;
const int maxLocalTime = 3;
RTC_DATA_ATTR int cntLocalTime = maxLocalTime;

#include <WiFiServerBasics.h>
WebServer server(80);

void goToSleep()
{
    //* uint64_t usec = timer.usecToSleep(t);
    uint64_t usec = 1000000 * 600;
    Serial.print("goToSleep(): us=");
    Serial.println(usec);
    //B delay(200);
    esp_sleep_enable_timer_wakeup(usec);
    esp_deep_sleep_start();
}

void getLocalTime()
{
    if (!getLocalTime(&t))
    {
        Serial.println("Failed to obtain time");
        cntLocalTime = maxLocalTime;
    }
    else
    {
        time(&now);
        Serial.printf("now: %ld -> ", now);
        now -= 5 * (cntLocalTime + 1);
        Serial.printf("now: %ld \n", now);
        localtime_r(&now, &t);

        Serial.println(&t, "%A, %B %d %Y %H:%M:%S");
        Serial.printf("%04d-%02d-%02d   ", 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday);
        Serial.printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
        time(&now);
        Serial.printf("time: %ld\n", now);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.printf("wake cnt: %d\n", ++cntLocalTime);

    if (cntLocalTime > maxLocalTime) // uzimanje vremena sa neta
    {
        ConnectToWiFi();
        cntLocalTime = 0;
    }
    configTime(1, 2 * 3600, ntpServer);
    getLocalTime();

    goToSleep();
}

void loop()
{
    delay(100);
}
