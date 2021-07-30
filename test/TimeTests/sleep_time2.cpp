// // za doradu pogledati: https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino

// #include <Arduino.h>

// #include "WiFiServerBasics.h"
// // #include "SpiffsUtils.h"

// #include "SleepTimer.h"
// RTC_DATA_ATTR SleepTimer timer(120, 4, -150);
// struct tm t;

// const int pinLed = 33;

// void statusLedON(bool on) { digitalWrite(pinLed, !on); }

// void setup()
// {
//     Serial.begin(115200);
//     Serial.println();
//     if (timer.isInitialized())
//         Serial.printf("Counter: %d/%ld\n", timer.getCountNetTimeCheck(), timer.getNetTimeCheck());
//     // SPIFFS.begin();
//     pinMode(pinLed, OUTPUT);
//     statusLedON(true);

//     if (timer.shouldGetNetTime())
//     {
//         ConnectToWiFi();
//         timer.getNetTime(t);
//         Serial.println("CoefError: " + String(timer.getCoefError()));
//         // SpiffsUtils::WriteFile("/times.log", String(timer.getTimerError()), "a");
//     }
//     else
//         timer.getLocalTime(t);
//     timer.waitFor00(t);

//     Serial.printf("%02d-%02d-%02d\n", t.tm_year, t.tm_mon, t.tm_mday);
//     Serial.printf("%02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
//     statusLedON(false);
//     esp_sleep_enable_timer_wakeup(timer.usecToSleep(t));
//     esp_deep_sleep_start();
// }

// void loop()
// {
//     delay(100);
// }