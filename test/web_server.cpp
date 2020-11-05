#include <Arduino.h>
#include <WiFiServerBasics.h>
WebServer server(80);

const int pinLed = 33;

void setup()
{
    Serial.begin(115200);
    pinMode(pinLed, OUTPUT);

    Serial.println("Start web server...");
    digitalWrite(pinLed, false);
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    SetupIPAddress(111);
    server.on("/txt", []() { server.send(200, "text/plain", "ESP is online..."); });
    server.on("/", []() { server.send(200, "text/html", "HeyHeyHey!"); });
    server.begin();
    digitalWrite(pinLed, true);
    Serial.println("Web server started.");
}

void loop()
{
    delay(10);
    server.handleClient();
}