//* https://randomnerdtutorials.com/esp32-cam-pir-motion-detector-photo-capture/

#include <Arduino.h>
#include <Blinky.h>
Blinky led(33, false, 250);

#include "FS.h"     // SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32

// RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    // Serial.begin(115200);

    // ++bootCount;
    // Serial.println("Boot number: " + String(bootCount));

    led.ledOn(true);
    delay(1000);

    // if (!SD_MMC.begin())
    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
    {
        led.blink(1000, 0);
        return;
    }

    File dir = SD_MMC.open("/test");
    if (dir)
        dir.close();
    else
        led.blink(1000, 0);
    SD_MMC.end();

    delay(1000);
    led.ledOn(false);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1);
    delay(1000);
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}