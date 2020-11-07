#pragma once

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#include <Arduino.h>
#include "esp_camera.h"

class AiThinkerCam
{
private:
    camera_config_t config;

    int flashLevel = 10;             // nivo osvetljenja: 0-ugaseno, 10-najjace svetlo
    int pinLed = 4;                  // ugradjeno jako LED svetlo
    static const int freq = 5000;    //
    static const int ledChannel = 2; // kanal 0 ne dozvoljava inicijalizaciju kamere
    static const int resolution = 8; // 

public:
    esp_err_t setup(framesize_t size, pixformat_t format);
    camera_fb_t *getFrameBuffer() { return esp_camera_fb_get(); }
    void returnFrameBuffer(camera_fb_t *fb) { esp_camera_fb_return(fb); }

    camera_config_t getConfig() { return config; }

    void setFlashLevel(int level, int pinLed = 4);
    void flashOn(bool val, int delayItv = 0);

    //* cuvanje slike na SD karticu
};
