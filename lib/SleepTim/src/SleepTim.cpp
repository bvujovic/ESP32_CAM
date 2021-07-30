#include "SleepTim.h"

SleepTim::SleepTim(long coefCorrect)
{
    if (!isInitialized())
        setCoefCorrect(coefCorrect);
}

SleepTim::SleepTim(long wakeEverySec, short netTimeCheck, long coefCorrect)
{
    if (!isInitialized())
    {
        this->wakeEverySec = wakeEverySec;
        this->maxNetTimeCheck = this->cntNetTimeCheck = netTimeCheck;
        setCoefCorrect(coefCorrect);
    }
}

long SleepTim::getNetTime(tm &t)
{
    long now = 0;
    //B configTime(1, 2 * 3600, NTP_RS);
    // configSntpTime();
    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "rs.pool.ntp.org");
    Serial.printf("ms2 %ld\n", millis());
    t = {0};
    bool success = getLocalTime(&t);
    Serial.printf("ms3 %ld\n", millis());
    if (success)
    {
        time(&now);
        Serial.printf("now=%ld\n", now);
    }
    else
        Serial.println("now fail!");
    // if (lastNetTime != 0)
    // {
    //     long prediction = nowLocal + (millis() - ms + 500) / 1000; // +500 je tu zbog zaokruzivanja umesto odsecanja
    //     coefError = (now == prediction) ? 0 : roundl((double)(now - lastNetTime) / (now - prediction));
    //     Serial.printf("razlika = %ld\n", now - prediction);
    // }
    if (success)
        lastNetTime = now;
    cntNetTimeCheck = success ? 0 : maxNetTimeCheck;
    return success ? now : 0;
}

// long SleepTim::getNetTime(tm &t)
// {
//     long nowLocal;
//     time(&nowLocal);
//     // Serial.printf("nowLocal=%ld\n", nowLocal);
//     ulong ms = millis();
//     long now = 0;
//     //B configTime(1, 2 * 3600, NTP_RS);
//     configSntpTime();
//     bool success = getLocalTime(&t);
//     if (success)
//     {
//         time(&now);
//         Serial.printf("now=%ld\n", now);
//     }
//     else
//         Serial.println("now fail!");
//     if (lastNetTime != 0)
//     {
//         long prediction = nowLocal + (millis() - ms + 500) / 1000; // +500 je tu zbog zaokruzivanja umesto odsecanja
//         coefError = (now == prediction) ? 0 : roundl((double)(now - lastNetTime) / (now - prediction));
//         Serial.printf("razlika = %ld\n", now - prediction);
//     }
//     if (success)
//         lastNetTime = now;
//     cntNetTimeCheck = success ? 0 : maxNetTimeCheck;
//     return success ? now : 0;
// }

long SleepTim::getTime(tm &t, bool correction)
{
    time_t now;
    time(&now);
    if (coefCorrect != 0 && correction)
    {
        int d = roundl((now - lastNetTime) / (float)coefCorrect);
        Serial.println("diff: " + String(now - lastNetTime) + ", d: " + String(d));
        now += d;
    }
    //B configTime(1, 2 * 3600, NTP_RS);
    configSntpTime();
    localtime_r(&now, &t);
    return now;
}

void SleepTim::setCoefCorrectIN(long coefCorrect)
{
    if (!isInitialized())
        setCoefCorrect(coefCorrect);
}

void SleepTim::setNetTimeCheck(long n)
{
    maxNetTimeCheck = n;
    if (!isInitialized())
        //B setCountNetTimeCheck();
        cntNetTimeCheck = maxNetTimeCheck;
}

uint64_t SleepTim::usecToSleep(tm &t)
{
    getLocalTime(&t);
    int sec = sleepSeconds(t.tm_min, t.tm_sec, wakeEverySec);
    //T Serial.println("sec: " + String(sec));
    return secToUsec(sec);
}

int SleepTim::sleepSeconds(int m, int s, int itv)
{
    if (itv % 60 != 0) // ako itv ne predstavlja tacan broj minuta, onda nema namestanja na pocetak minuta
        return itv;
    int x = itv / 60;            // 300/60 = 5
    int nextMin = m / x * x + x; // 56/5 * 5 + 5 = 11*5 + 5 = 55 + 5 = 60
    int min = nextMin - m;       // 60 - 56 = 4
    int sec = min * 60 - s;      // 4*60 - 30 = 240 - 30 = 210
    return sec;
}

void SleepTim::waitFor00(tm &t)
{
    if (wakeEverySec % 60 != 0)
        return;
    while (true)
    {
        getLocalTime(&t);
        if (t.tm_sec < 45)
            break;
        delay(250);
        //T Serial.print('#');
    }
}
