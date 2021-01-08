#include <Arduino.h>

#define uS_TO_S 1000000

RTC_DATA_ATTR long x = 0;

struct tm t;
class Cls
{
private:
    // tm tx; //! dodavanje ovog elementa klasi sprecava cuvanje polja u RTC memoriji
public:
    int x = 0;
    int m() { return x; };
    struct tm &ms(struct tm &ti)
    {
        ti.tm_year = 2021;
        ti.tm_mon = 1;
        ti.tm_mday = x;
        return ti;
    };
};
RTC_DATA_ATTR Cls c;

// class Clss
// {
// private:
//     int x = 0;
//     tm ti; //! dodavanje ovog elementa klasi sprecava cuvanje polja x u RTC memoriji
// public:
//     void inc() { x += 4; }
//     void disp() { Serial.println(x); }
// };
// RTC_DATA_ATTR Clss s;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    x++;
    c.x += 2;

    Serial.println(x);
    Serial.println(c.ms(t).tm_mday);

    esp_sleep_enable_timer_wakeup(uS_TO_S * ((x <= 2) ? 5 : 1000));
    esp_deep_sleep_start();
}

void loop() {}