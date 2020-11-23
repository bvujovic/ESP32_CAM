#include <Arduino.h>
#define TEST true
#include <UtilsCommon.h>

#include <WiFiServerBasics.h>
WebServer server(80);

#include <Blinky.h>
Blinky blnk(33, false);

#include <AiThinkerCam.h>
AiThinkerCam cam;

#include "SD_MMC.h" // SD Card ESP32

void PreviewHandler()
{
    tprintln("preview");

    // esp_err_t res = cam.setup(FRAMESIZE_QVGA, PIXFORMAT_JPEG);
    // tprintln("Camera...");
    // tprintln(esp_err_to_name(res));

    // if (fb)
    // {
    //     tprintln("return fb");
    //     cam.returnFrameBuffer(fb);
    // }

    ulong ms = millis();
    camera_fb_t *fb = cam.getFrameBuffer();
    if (!fb)
        server.send_P(404, "text/plain", "Camera capture failed");
    else
    {
        server.send_P(200, "image/jpeg", (char *)fb->buf, fb->len);
        tprintln(millis() - ms);
        cam.returnFrameBuffer(fb);
    }
}

void SettingsHandler()
{
    tprintln("settings");
    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, (framesize_t)server.arg("res").toInt());
    s->set_brightness(s, server.arg("bright").toInt());
    s->set_gainceiling(s, (gainceiling_t)server.arg("gain").toInt());
    SendEmptyText(server);
}

void ListSdCardHandler()
{
    String str;
    File dir = SD_MMC.open("/");
    if (dir)
    {
        File f;
        while (f = dir.openNextFile())
        {
            if (strstr(f.name(), ".jpg")) // samo me slike interesuju
                (str += f.name()) += "\n";
            f.close();
        }
        dir.close();
        server.send(200, "text/x-csv", str);
    }
    else
        tprintln("dir root fail");
}

void SdCardImgHandler()
{
    File f = SD_MMC.open("/" + server.arg("img"), "r");
    if (f)
    {
        server.streamFile(f, "image/jpeg");
        f.close();
    }
    else
    {
        server.send_P(404, "text/plain", "Error reading file");
        tprintln(server.arg("img") + " - error reading file.");
    }
}

// Konektovanje na WiFi, postavljanje IP adrese i startovanje veb servera.
void WiFiOn()
{
    tprintln("Turning WiFi ON...");
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    SetupIPAddress(60);
    // server.on("/text_editor.html", []() { HandleDataFile(server, "/text_editor.html", "text/html"); });
    // void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL );
    server.on("/", []() { HandleDataFile(server, "/index.html", "text/html"); });
    server.on("/webcam.png", []() { delay(3000); HandleDataFile(server, "/webcam.png", "image/png"); });
    server.on("/preview", PreviewHandler);
    server.on("/settings", SettingsHandler);
    server.on("/txt", []() { server.send(200, "text/plain", "ESP is online..."); });
    server.on("/listSdCard", ListSdCardHandler);
    server.on("/sdCardImg", SdCardImgHandler);

    server.begin();
    tprintln("WiFi ON");
}

void setup()
{
    blnk.ledOn(true);
    tbegin(115200);
    SPIFFS.begin();
    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
    {
        tprintln("SD Card Mount Failed");
        blnk.blink(250, 0);
    }

    WiFiOn();

    esp_err_t res = cam.setup(FRAMESIZE_QVGA, PIXFORMAT_JPEG);
    tprintln("Camera...");
    tprintln(esp_err_to_name(res));

    blnk.ledOn(false);
}

void loop()
{
    server.handleClient();
    delay(100);
}