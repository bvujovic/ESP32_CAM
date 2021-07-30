// // https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/

// #include "time.h"
// #include "WiFiServerBasics.h"

// const char *ntpServer = "rs.pool.ntp.org";
// tm t;
// time_t now;

// // Diskonektovanje sa WiFi-a.
// void WiFiOff()
// {
//     Serial.println("Turning WiFi OFF...");
//     WiFi.disconnect();
//     WiFi.mode(WIFI_OFF);
//     delay(100);
//     Serial.println("WiFi OFF");
// }

// void getLocalTime()
// {
//     ulong ms = millis();
//     if (!getLocalTime(&t))
//         Serial.println("Failed to obtain time");
//     else
//     {
//         Serial.printf("%04d-%02d-%02d   ", 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday);
//         Serial.printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
//         Serial.println(&t, "%A, %B %d %Y %H:%M:%S");
//         time(&now);
//         Serial.print("time: ");
//         Serial.println(now);
//         Serial.println(millis() - ms);
//     }
// }

// void setup()
// {
//     Serial.begin(115200);
//     ConnectToWiFi();
//     configTime(1, 3600, ntpServer);
//     getLocalTime();
//     WiFiOff();
// }

// void loop()
// {
//     delay(5000);
//     getLocalTime();
// }