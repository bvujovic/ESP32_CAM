//* https://randomnerdtutorials.com/esp32-cam-send-photos-email/

#include "ESP32_MailClient.h"

#include "WiFiServerBasics.h"

#include <Blinky.h>
Blinky led = Blinky::createESP();

#include "SD_MMC.h" // SD Card ESP32

// YOU MUST ENABLE less secure app option https://myaccount.google.com/lesssecureapps?pli=1
#include "CredGmailAdvokat.h"
#define smtpServer "smtp.gmail.com"
#define smtpServerPort 465
#define emailSubject "ESP32-CAM Photo"
#define emailRecipient "bojan.prog@gmail.com"

// The Email Sending data object contains config and data to send
SMTPData smtpData;

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/aq.jpg"

// Callback function to get the Email sending status
void sendCallback(SendStatus msg) { Serial.println(msg.info()); }

void sendPhoto()
{
    Serial.println("Sending email...");
    smtpData.setLogin(smtpServer, smtpServerPort, gmailAdvokatUser, gmailAdvokatPass);

    smtpData.setSender("ESP32-CAM", gmailAdvokatUser);
    smtpData.setPriority(3); // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
    smtpData.setSubject(emailSubject);
    smtpData.setMessage("<h2>Photo captured with ESP32-CAM and attached in this email.</h2>", true);
    //smtpData.setMessage("Photo captured with ESP32-CAM and attached in this email.", false);
    smtpData.addRecipient(emailRecipient); // Add recipients, can add more than one recipient

    smtpData.addAttachFile(FILE_PHOTO, "image/jpg");
    // Set the storage type to attach files in your email (SPIFFS)
    smtpData.setFileStorageType(MailClientStorageType::SPIFFS);
    smtpData.setSendCallback(sendCallback);
    if (!MailClient.sendMail(smtpData))
    {
        Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
        led.blinkErrorMajor();
    }
    smtpData.empty(); // Clear all data from Email object to free memory
}

void setup()
{
    Serial.begin(115200);
    Serial.println();

    // Connect to Wi-Fi
    ConnectToWiFi();

    if (!WiFi.isConnected())
        led.blinkErrorMajor();

    // Print ESP32 Local IP Address
    Serial.print("IP Address: http://");
    Serial.println(WiFi.localIP());

    // if (!SD_MMC.begin("/sdcard", true)) // 1-bitni mod
    // {
    //     Serial.println("SD Card Mount Failed");
    //     led.blinkErrorMajor();
    // }
    // File dir = SD_MMC.open("/test");
    // if (dir)
    // {
    //     Serial.println("Dir /test opened!");
    //     dir.close();
    // }
    // else
    // {
    //     Serial.println("Dir /test NOT opened!");
    //     led.blinkErrorMinor();
    // }

    sendPhoto();
    SD_MMC.end();
}

void loop()
{
    delay(100);
}
