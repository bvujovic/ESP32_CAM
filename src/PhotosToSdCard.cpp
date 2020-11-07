#include <Arduino.h>

#include <Blinky.h>
Blinky blnk(33, false);

#include "AiThinkerCam.h"
AiThinkerCam cam;

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
    //B char *srvr = (char *)malloc(50);
    char *srvr = new char[50];
    strcpy(srvr, "rs.pool.ntp.org");
    //B sntp_setservername(0, "rs.pool.ntp.org");
    sntp_setservername(0, srvr);
    sntp_init();
    //*** time_t now = 0;
    ti = {0};
    int retry = 0;
    const int retry_count = 10;
    while (ti.tm_year < 2020 && ++retry < retry_count)
    {
        Serial.printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        delay(2000);
        getTime();
    }
}

#include <WiFiServerBasics.h>
bool isWiFiOn = false;

// Konektovanje na WiFi, uzimanje tacnog vremena, postavljanje IP adrese i startovanje veb servera.
void wiFiOn()
{
    blnk.ledOn(true);
    Serial.println("Turning WiFi ON...");
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    initTime();
    SetupIPAddress(60);
    // server.on("/", []() { HandleDataFile(server, "/index.html", "text/html"); });
    // server.on("/inc/comp_on-off_btn1.png", []() { HandleDataFile(server, "/inc/comp_on-off_btn1.png", "image/png"); });
    // server.on("/inc/script.js", []() { HandleDataFile(server, "/inc/script.js", "text/javascript"); });
    // server.on("/inc/style.css", []() { HandleDataFile(server, "/inc/style.css", "text/css"); });
    // server.on("/dat/config.ini", []() { HandleDataFile(server, "/dat/config.ini", "text/x-csv"); });
    // server.on("/save_config", HandleSaveConfig);
    // server.begin();
    isWiFiOn = true;
    Serial.println("WiFi ON");
    blnk.ledOn(false);
}

// Diskonektovanje sa WiFi-a.
void wiFiOff()
{
    isWiFiOn = false;
    Serial.println("Turning WiFi OFF...");
    // server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(100);
    Serial.println("WiFi OFF");
}

ulong ms;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    blnk.blink2(300, 1200, 3);

    char path[50];
    sprintf(path, "/pic%03d.bmp", 2);
    Serial.println(path);

    wiFiOn();
    wiFiOff();

    Serial.println(ti.tm_sec);
    delay(5000);
    getTime();
    Serial.println(ti.tm_sec);
    sprintf(path, "/picture_%02d-%02d-%02d_%02d:%02d.bmp", ti.tm_year, ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min);
    Serial.println(path);

    while (true)
        delay(100);

    esp_err_t err = cam.setup(FRAMESIZE_QVGA, PIXFORMAT_GRAYSCALE);
    if (err != ESP_OK)
    {
        Serial.print("Greska pri inicijalizaciji kamere: ");
        Serial.println(esp_err_to_name(err));
        blnk.blink(1000, 0);
    }
}

void loop()
{
    
    delay(6000);
}
