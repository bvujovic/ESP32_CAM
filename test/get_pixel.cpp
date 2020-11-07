#include <Arduino.h>

#include <Blinky.h>
Blinky blnk(33, false);

#include "AiThinkerCam.h"
AiThinkerCam cam;

ulong ms;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    blnk.blink2(300, 1200, 3);

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
    blnk.blink(500, 1);

    cam.flashOn(true, 100);
    camera_fb_t *fb = cam.getFrameBuffer();
    cam.flashOn(false);
    if (!fb)
    {
        Serial.println("Camera capture failed");
        blnk.blink(250, 8);
        return;
    }
    else
    {
        Serial.println(fb->len);
        ms = millis();
        size_t sum = 0;
        for (size_t i = 0; i < fb->len; i++)
            sum += fb->buf[i];
        Serial.println(millis() - ms); // 5-6ms treba za racunanje proseka
        ms = millis();
        Serial.println("Avg pix: ");
        Serial.println((float)sum / (float)fb->len);
        Serial.println();
    }
    cam.returnFrameBuffer(fb);
    delay(6000);
}
