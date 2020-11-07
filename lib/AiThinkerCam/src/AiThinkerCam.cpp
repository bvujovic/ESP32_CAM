#include "AiThinkerCam.h"
#include <esp32-hal-psram.h>

esp_err_t AiThinkerCam::setup(framesize_t size, pixformat_t format)
{
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = format;

    if (psramFound())
    {
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    config.frame_size = size;

    // config strong LED (flash)
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(pinLed, ledChannel);

    return esp_camera_init(&config);
}

void AiThinkerCam::setFlashLevel(int level, int pinLed)
{
    flashLevel = constrain(level, 0, 10);
    this->pinLed = pinLed;
}

void AiThinkerCam::flashOn(bool val, int delayItv)
{
    ledcWrite(ledChannel, val ? flashLevel : 0);
    if (delayItv > 0)
        delay(delayItv);
}
