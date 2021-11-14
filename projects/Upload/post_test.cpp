//* Konacna verzija slanja POST zahteva na Azure web app (https).

#include <Arduino.h>
#include <WiFiServerBasics.h>
#include "soc/rtc_cntl_reg.h"
#include <WiFiClientSecure.h>
WiFiClientSecure client;
String serverName = "imageuploadbv.azurewebsites.net";
IPAddress ipAddress(13, 69, 68, 18);

#include <Blinky.h>
Blinky led = Blinky::create();

#include <AiThinkerCam.h>
AiThinkerCam cam;
const ulong timerInterval = 30000; // time between each HTTP POST image
ulong msPrevious = 0;              // last time image was sent

void sendPhoto();

void FreeMemKB(const char *caption) { Serial.printf("%s\t%d\n", caption, ESP.getFreeHeap() / 1024); }

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    Serial.begin(115200);
    delay(3000);
    FreeMemKB("esp init");

    ConnectToWiFi();

    esp_err_t err = cam.setup(framesize_t::FRAMESIZE_SVGA, PIXFORMAT_JPEG);
    if (err != ESP_OK)
        led.printBlink("Kamera nece da se inicijalizuje.");
    FreeMemKB("cam init");

    client.setInsecure();
    sendPhoto();

    led.printBlink("dosta zajebancije!");
}

void loop()
{
    ulong ms = millis();
    //B if (currentMillis - previousMillis >= timerInterval)
    if (ms > msPrevious + timerInterval)
    {
        msPrevious = ms;
        sendPhoto();
    }
}

void sendPhoto()
{
    String getAll;
    String getBody;

    camera_fb_t *fb = cam.getFrameBuffer();
    if (!fb)
        led.printBlink("Camera capture failed");

    Serial.println("Connecting to server...");
    // if (!client.connect(serverName.c_str(), 443))
    if (!client.connect(ipAddress, 443))
        led.printBlink("Connection to server failed.");

    Serial.println("Connection successful!");
    String head = "--boundary\r\n"
                  //   "Content-Disposition: form-data; name=\"Name\"\r\n\r\nt.txt\r\n--boundary\r\n"
                  //   "Content-Disposition: form-data; name=\"File\"; filename=\"zec.txt\"\r\n\r\n";
                  //   "Content-Disposition: form-data; name=\"Name\"\r\n\r\nzec.jpg\r\n--boundary\r\n"
                  "Content-Disposition: form-data; name=\"Camera\"\r\n\r\nPost Test\r\n--boundary\r\n"
                  "Content-Disposition: form-data; name=\"File\"; filename=\"cam.jpg\"\r\n"
                  "Content-Type: image/jpeg\r\n\r\n";
    // --boundary  Content-Disposition: form-data; name="Name"    test.jpg  --boundary  Content-Disposition: form-data; name="File"; filename="test.jpg"
    String tail = "\r\n--boundary--\r\n";

    // const char *payload = "Peti\r\nKuli\r\nZec :)";
    // uint32_t imageLen = strlen(payload);
    uint32_t imageLen = fb->len;
    Serial.printf("Image length: %u\n", imageLen);
    uint32_t totalLen = imageLen + head.length() + tail.length();
    client.println("POST /api/ImgUpload HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Connection: close");
    client.println("Content-Type: multipart/form-data;boundary=\"boundary\"");
    client.println("Content-Length: " + String(totalLen) + "\r\n");

    client.print(head);

    uint8_t *fbBuf = fb->buf;
    // uint8_t *fbBuf = (uint8_t *)payload;
    const size_t chunk = 4096; //OG 1024
    for (size_t n = 0; n < imageLen; n += chunk)
    {
        if (n + chunk < imageLen)
        {
            client.write(fbBuf, chunk);
            fbBuf += chunk;
        }
        else if (imageLen % chunk > 0)
        {
            size_t remainder = imageLen % chunk;
            client.write(fbBuf, remainder);
        }
    }
    client.print(tail);
    cam.returnFrameBuffer(fb);

    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + timoutTimer) > millis())
    {
        Serial.print(".");
        delay(100);
        while (client.available())
        {
            char c = client.read();
            //T Serial.print(c);
            if (c == '\n')
            {
                if (getAll.length() == 0)
                    state = true;
                getAll = "";
            }
            else if (c != '\r')
                getAll += String(c);
            if (state == true)
                getBody += String(c);
            startTimer = millis();
        }
        if (getBody.length() > 0)
            break;
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
}
