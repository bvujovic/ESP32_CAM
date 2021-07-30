#pragma once

#include <Arduino.h>
#include "time.h"
#include "lwip/apps/sntp.h"

#define NTP_RS "rs.pool.ntp.org"

//
class SleepTim
{
private:
    long lastNetTime;
    long coefCorrect;
    long coefError;
    long wakeEverySec;     // Na koliko sekundi se aparat budi.
    short cntNetTimeCheck; // Brojac budjenja [0..maxNetTimeCheck]
    short maxNetTimeCheck; // Posle koliko budjenja se uzima vreme sa neta radi provere.
    //
    void configSntpTime()
    {
        configTime(1, 2 * 3600, NTP_RS); //* drugi param bi trebao biti 3600, a ne duplo, ali ovako radi
    }
    int sleepSeconds(int m, int s, int itv);

public:
    SleepTim() {}
    SleepTim(long coefCorrect);
    SleepTim(long wakeEverySec, short netTimeCheck, long coefCorrect);
    // Uzimanje tacnog vremena sa SNTP servera. WiFi mora biti ukljucen. Vraca POSIX broj sekundi.
    long getNetTime(tm &t);
    // Izracunavanje tacnog vremena na osnovu poslednjeg SNTP vremena i internog sata. Vraca POSIX broj sekundi.
    long getTime(tm &t, bool correction = true);
    // Da li je do sada pozvan getNetTime() i time uzeto tacno vreme sa neta.
    bool isInitialized() { return lastNetTime != 0; }
    //
    long getCoefCorrect() { return coefCorrect; }
    //
    void setCoefCorrect(long coefCorrect) { this->coefCorrect = coefCorrect; }
    // Postavljanje koeficijenta za korigovanje vremena samo ako objekat nije inicijalizovan (lastNetTime==0).
    void setCoefCorrectIN(long coefCorrect);
    // Vraca koeficijent greske sata NEracunajuci postavljen coefCorrect. Proteklo vreme (sec) / greska (sec).
    long getCoefError() { return coefError; }
    // Na koliko sekundi se aparat budi.
    long getWakeEvery() { return wakeEverySec; }
    // Postavlja broj sekundi na koliko se aparat budi.
    void setWakeEvery(long sec) { wakeEverySec = sec; }
    // Posle koliko budjenja se uzima vreme sa neta.
    long getNetTimeCheck() { return maxNetTimeCheck; }
    //
    bool shouldGetNetTime() { return ++cntNetTimeCheck >= maxNetTimeCheck; }
    // Postavljanje broja budjenja posle koliko se uzima vreme sa neta.
    void setNetTimeCheck(long n);
    //
    short getCountNetTimeCheck() { return cntNetTimeCheck + 1; }
    //B void setCountNetTimeCheck(long n) { cntNetTimeCheck = n; Serial.println("cntNetTimeCheck promena 3"); }
    //B void setCountNetTimeCheck() { cntNetTimeCheck = maxNetTimeCheck; Serial.println("cntNetTimeCheck promena 3.5"); }
    // Za dati broj sekundi, staticki metod vraca broj mikrosekundi.
    uint64_t static secToUsec(int sec) { return (uint64_t)1000 * 1000 * sec; }
    //
    uint64_t usecToSleep(tm &t);
    //
    void waitFor00(tm &t);
};
