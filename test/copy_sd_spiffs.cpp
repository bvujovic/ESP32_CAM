//* Kopiranje fajla sa SD kartice na SPIFFS
//* Vreme kopiranja fajla od oko 80kB je ~1sec.
//* https://www.reddit.com/r/esp32/comments/b9l8kb/copying_file_sd_spiffs_arduino_ide/

#include <Arduino.h>
#include "SD_MMC.h"
#include <SPIFFS.h>

const char *fileName = "/aqq.jpg";

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Pocetak: " + String(millis()));

    SPIFFS.begin();
    if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
        Serial.println("SD Card Mount Failed");

    if (!SD_MMC.exists(fileName))
        Serial.println("Nema fajla!");

    File fpSource = SD_MMC.open(fileName, "r");
    if (fpSource)
    {
        File fpDest = SPIFFS.open(fileName, FILE_WRITE);
        uint8_t buf[512];
        while (fpSource.read(buf, 512))
            fpDest.write(buf, 512);
        fpDest.close();
        fpSource.close();
    }

    if (SPIFFS.exists(fileName))
    {
        Serial.println("Fajl postoji na SPIFFSu");
        File fp = SPIFFS.open(fileName);
        Serial.println("Velicina fajla: " + String(fp.size()));
    }
    else
        Serial.println("Fajl NE postoji na SPIFFSu");

    SD_MMC.end();
    SPIFFS.end();
    Serial.println("Kraj: " + String(millis()));
}

void loop()
{
    delay(100);
}
