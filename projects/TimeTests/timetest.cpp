#include <Arduino.h>
#include <time.h>
#include <WiFiServerBasics.h>

const char* NTP_SERVER = "rs.pool.ntp.org";
//B const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";
const char* TZ_INFO    = "CET-1CEST,M3.5.0,M10.5.0/3";

void setup()
{
    Serial.begin(115200);
    Serial.println();

    ConnectToWiFi();

    Serial.println(1);

    long now = 1612135500; // Mon Feb 01 2021 00:25:00 GMT+0100
    tm t;
    Serial.println(2);

    // configTime(0, 0, NTP_SERVER);
    // Serial.println(3);
    configTzTime(TZ_INFO, NTP_SERVER);
    Serial.println(4);
    // setenv("TZ", TZ_INFO, 1); // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes
    localtime_r(&now, &t);
    Serial.println(5);

    t.tm_year += 1900;
    t.tm_mon++;
    // t.tm_hour++;
    // if (t.tm_hour == 24)
    // {
    //     t.tm_hour = 0;
    //     t.tm_mday++;
    // }

    Serial.printf("%02d-%02d-%02d\n", t.tm_year, t.tm_mon, t.tm_mday);
    Serial.printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);

    delay(4000);
    
    // time_t now;
    time(&now);
    localtime_r(&now, &t);
    Serial.printf("%02d-%02d-%02d\n", t.tm_year, t.tm_mon, t.tm_mday);
    Serial.printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);

    while (true)
        delay(100);
}

void loop()
{
}