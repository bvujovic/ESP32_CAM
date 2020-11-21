#include <Arduino.h>
#define TEST true
#include <UtilsCommon.h>

#include <WiFiServerBasics.h>
WebServer server(80);

#include <Blinky.h>
Blinky blnk(33, false);

#include <AiThinkerCam.h>
AiThinkerCam cam;
// camera_fb_t *fb = NULL;

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

// Konektovanje na WiFi, postavljanje IP adrese i startovanje veb servera.
void WiFiOn()
{
    tprintln("Turning WiFi ON...");
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    SetupIPAddress(60);
    // server.on("/", []() { HandleDataFile(server, "/index.html", "text/html"); });
    // server.on("/inc/vehicle.png", []() { HandleDataFile(server, "/inc/vehicle.png", "image/png"); });
    // server.on("/inc/script.js", []() { HandleDataFile(server, "/inc/script.js", "text/javascript"); });
    // server.on("/text_editor.html", []() { HandleDataFile(server, "/text_editor.html", "text/html"); });
    // application/json

    // void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL );
    server.on("/", []() { HandleDataFile(server, "/index.html", "text/html"); });
    server.on("/webcam.png", []() { HandleDataFile(server, "/webcam.png", "image/png"); });
    server.on("/preview", PreviewHandler);
    server.on("/settings", SettingsHandler);
    server.on("/txt", []() { server.send(200, "text/plain", "ESP is online..."); });

    server.begin();
    tprintln("WiFi ON");
}

void setup()
{
    blnk.ledOn(true);
    tbegin(115200);
    SPIFFS.begin();

    // ReadConfigIni();

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