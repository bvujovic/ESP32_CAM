#include <Arduino.h>
#include <Blinky.h>
Blinky led = Blinky::create();

#include "WiFiServerBasics.h"
#include "ESP32_MailClient.h"
#include "CredGmailAdvokat.h"
SMTPData smtpData;

#include "driver/rtc_io.h"

#include "SD_MMC.h"
#define FILE_PHOTO "/aqq.jpg"
#include <AiThinkerCam.h>
AiThinkerCam cam;

void sendCallback(SendStatus msg) { Serial.println(msg.info()); }

bool sendPhoto(const String &fileName, uint8_t storageType)
{
    if (storageType == MailClientStorageType::SPIFFS)
    {
        File fp = SPIFFS.open(FILE_PHOTO, FILE_READ);
        if (!fp)
            return false;
        if (fp.size() == 0)
        {
            Serial.println("Pic size is 0.");
            return false;
        }
    }
    smtpData.setLogin(gmailSmtpServer, gmailSmtpServerPort, gmailAdvokatUser, gmailAdvokatPass);
    smtpData.setSender("ESP32-CAM", gmailAdvokatUser);
    smtpData.setPriority(3);
    smtpData.setSubject("ESP32-CAM Photo");
    smtpData.setMessage("<h2>Photo captured with ESP32-CAM and attached in this email.</h2>", true);
    smtpData.addRecipient("bojan.prog@gmail.com");
    //* smtpData.addRecipient("bv.net@outlook.com");
    smtpData.addAttachFile(fileName, "image/jpg");
    smtpData.setFileStorageType(storageType);
    smtpData.setSendCallback(sendCallback);
    bool res = MailClient.sendMail(smtpData);
    smtpData.empty();
    return res;
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    led.blinkOk();

    //* slikanje
    esp_err_t err = cam.setup(framesize_t::FRAMESIZE_SVGA, PIXFORMAT_JPEG);
    if (err == ESP_OK)
    {
        // sensor_t *s = esp_camera_sensor_get();
        // s->set_brightness(s, setts.brightness);
        // s->set_gainceiling(s, (gainceiling_t)setts.gain);
    }
    else
        Serial.println("cam.setup() fejlovao");

    delay(2000);
    camera_fb_t *fb = cam.getFrameBuffer();
    if (!fb)
        Serial.println("slikanje fejlovalo!");
    else
    {
        SPIFFS.begin();
        File fp = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
        if (!fp)
            Serial.println("fejl cuvanja slike na SPIFFSu.");
        else
        {
            fp.write(fb->buf, fb->len);
            fp.close();
        }
        cam.returnFrameBuffer(fb);
        cam.deinit();
    }

    //* slanje na mejl
    ConnectToWiFi();
    if (!sendPhoto(FILE_PHOTO, MailClientStorageType::SPIFFS))
        Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    Serial.println("slanje na mejl zavrseno");
    SPIFFS.end();

    //* slikanje na svakih x sec i stavljanje na kt.info i/ili sd karticu
    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
        Serial.println("SD Card Mount Failed");
    Serial.println("sd card ****** cam init");
    err = cam.setup(framesize_t::FRAMESIZE_XGA, PIXFORMAT_JPEG);
    Serial.println("setup cam: " + String(err != ESP_OK));
    char imgName[15];
    int cntImgName = 1;

    while (millis() < 30000)
    {
        led.on();
        camera_fb_t *fb = cam.getFrameBuffer();
        if (!fb)
            Serial.println("slikanje fejlovalo!");
        else
        {
            Serial.println("pic" + String(cntImgName));
            sprintf(imgName, "/pic%d.jpg", cntImgName++);
            File fp = SD_MMC.open(imgName, FILE_WRITE);
            if (!fp)
                Serial.println("fejl cuvanja slike na SD_MMCu.");
            else
            {
                fp.write(fb->buf, fb->len);
                fp.close();
            }
            cam.returnFrameBuffer(fb);
        }
        led.off();
        delay(3000);
    }
    cam.deinit();
    SD_MMC.end();

    // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
    gpio_num_t pinLedFlash = GPIO_NUM_4;
    pinMode(pinLedFlash, OUTPUT);
    digitalWrite(pinLedFlash, LOW);
    rtc_gpio_hold_en(pinLedFlash);

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
    esp_deep_sleep_start();
}

void loop()
{
}