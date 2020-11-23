#include <Arduino.h>

#include "FS.h"     // SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32

void setup()
{
    Serial.begin(115200);

    Serial.println("Starting SD Card");
    //B if (!SD_MMC.begin())
    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
    {
        Serial.println("SD Card Mount Failed");
        return;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD Card attached");
        return;
    }

    fs::FS &fs = SD_MMC;
    // Serial.print("Create test dir: ");
    // Serial.println(fs.mkdir("/test"));

    int totalBytes = SD_MMC.totalBytes() / 1024;
    Serial.print(totalBytes);
    Serial.println(" KB");
    int usedBytes = SD_MMC.usedBytes() / 1024;
    Serial.print(usedBytes);
    Serial.println(" KB");

    File dir = fs.open("/");
    // File dir = fs.open("/test");
    if (dir)
    {
        Serial.println(dir.isDirectory());
        Serial.println(dir.name());
        Serial.println(dir.size());

        File f;
        while (f = dir.openNextFile())
        {
            Serial.print(f.name());
            Serial.print('\t');
            Serial.println(f.size() / 1024);
            f.close();
        }
        dir.close();
    }
    else
        Serial.println("dir fail");

    // File f3 = fs.open("/test/f.txt");
    // File f2 = fs.open("/test.txt");
    // if (f2)
    // {
    //     Serial.println(f2.isDirectory());
    //     Serial.println(f2.readString());
    //     Serial.println(f2.size());
    //     f2.close();
    // }
    // else
    //     Serial.println("f2 fail");

    SD_MMC.end();
}

void loop()
{
    delay(100);
}