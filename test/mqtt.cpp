#include <Arduino.h>
#include <WiFiServerBasics.h>
#include <PubSubClient.h>
#include "CredAdafruit.h"
#define AIO_FEED "surveillance-cam-pir"

WiFiClient wiFiClient;
PubSubClient mqtt(wiFiClient);
ulong ms;

#include <Blinky.h>
Blinky led = Blinky::createESP();

void setup()
{
    led.ledOn(true);
    WiFi.mode(WIFI_STA);
    delay(1000);
    ConnectToWiFi();
    if (!WiFi.isConnected())
        led.blinkErrorMajor();
    mqtt.setServer(AIO_SERVER, AIO_SERVERPORT);
    led.ledOn(false);
    ms = millis();
}

void loop()
{
    delay(100);

    if (millis() - ms > 10000)
    {
        while (!mqtt.connect("", AIO_USER, AIO_KEY))
            led.blinkIrregular(250, 750, 2);
        bool succ = mqtt.publish(AIO_USER AIO_FEEDS AIO_FEED, "1");
        if (succ)
        {
            led.blinkOk();
            mqtt.publish(AIO_USER AIO_FEEDS AIO_FEED, "0");
        }
        else
            led.blinkErrorMinor();
        ms = millis();
    }
}