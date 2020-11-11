//* Aparat sa ovim skecom bi mogao da snima (slika svakih 5min) neki deo stana.
//* Slike se cuvaju na SD kartici pa korisnik moze da vidi sta je radio tokom dana.

#include <Arduino.h>
#define TEST true
#include <UtilsCommon.h>

#include <Blinky.h>
Blinky blnk(33, false);

#include "AiThinkerCam.h"
AiThinkerCam cam;

#include "FS.h"
#include "SD_MMC.h"
char filePath[50];

#include "time.h"
//#include "lwip/err.h"
#include "lwip/apps/sntp.h"
struct tm ti;
time_t now;

void getTime()
{
    time(&now);
    localtime_r(&now, &ti);
    ti.tm_year += 1900;
    ti.tm_mon += 1;
    ti.tm_hour += 1;
}

void initTime()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    char *srvr = new char[30];
    strcpy(srvr, "rs.pool.ntp.org");
    sntp_setservername(0, srvr);
    sntp_init();
    ti = {0};
    int retry = 0;
    const int retry_count = 5;
    while (ti.tm_year < 2020 && ++retry < retry_count)
    {
        tprintln("Waiting for system time to be set...");
        delay(2000);
        getTime();
    }
}

#include <WiFiServerBasics.h>
bool isWiFiOn = false;

// Konektovanje na WiFi, uzimanje tacnog vremena, postavljanje IP adrese i startovanje veb servera.
void wiFiOn()
{
    tprintln("Turning WiFi ON...");
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    initTime();
    SetupIPAddress(60);
    isWiFiOn = true;
    tprintln("WiFi ON");
}

// Diskonektovanje sa WiFi-a.
void wiFiOff()
{
    isWiFiOn = false;
    tprintln("Turning WiFi OFF...");
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(100);
    tprintln("WiFi OFF");
}

RTC_DATA_ATTR int bootCount = 0;
ulong ms;
uint64_t secToUsec(int sec) { return (uint64_t)1000 * 1000 * sec; }

// Temporary problem. ESP will try to continue after reset.
void errorTemp()
{
    blnk.blink(500, 5);
    esp_sleep_enable_timer_wakeup(secToUsec(10));
    esp_deep_sleep_start();
}

// Fatal error. ESP cannot continue working properly.
void errorFatal() { blnk.blink(1000, 0); }

void setup()
{
#if TEST
    blnk.ledOn(true);
#endif
    ms = millis();
    tbegin(115200);

    if (bootCount++ == 0)
    {
        wiFiOn();
        wiFiOff();
    }

    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
    {
        tprintln("SD Card Mount Failed");
        errorFatal();
    }

    esp_err_t err = cam.setup(FRAMESIZE_SXGA, PIXFORMAT_JPEG);
    if (err != ESP_OK)
    {
        tprintln("Greska pri inicijalizaciji kamere: ");
        tprintln(esp_err_to_name(err));
        errorTemp();
    }

    camera_fb_t *fb = cam.getFrameBuffer();
    if (!fb)
    {
        tprintln("Camera capture failed");
        errorTemp();
    }

    getTime();
    sprintf(filePath, "/pic_%02d-%02d-%02d_%02d.%02d.%02d.jpg", ti.tm_year, ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
    tprintln(filePath);

    File file = SD_MMC.open(filePath, FILE_WRITE);
    if (!file)
    {
        tprintln("Failed to open file in writing mode");
        errorTemp();
    }
    else
    {
        tprintln("Writting file...");
        file.write(fb->buf, fb->len);
        tprintln("File written.");
        file.close();
    }
    cam.returnFrameBuffer(fb);
    SD_MMC.end();

#if TEST
    blnk.ledOn(true);
#endif
    esp_sleep_enable_timer_wakeup(secToUsec(TEST ? 10 : 5 * 60));
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}
