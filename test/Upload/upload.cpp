//* Upload slike na veb server.
//* https://randomnerdtutorials.com/esp32-cam-http-post-php-arduino/
//* Ne radi uvek. Ponekad prikaze: [E][WiFiClient.cpp:258] connect(): socket error on fd 54, errno: 113,
//* "Software caused connection abort" Connection to kingtrader.info failed.
//* Mozda bi se moglo probati sa oslobadjanjem memorije za kameru: slikanje->slika->spiffs->deinit->upload

#include <Arduino.h>
#include <WiFiServerBasics.h>
#include <AiThinkerCam.h>
AiThinkerCam cam;

#include <Blinky.h>
Blinky led = Blinky::create();

String serverName = "kingtrader.info";
IPAddress serverIP(185, 212, 108, 195);
String serverPath = "/php/upload.php";
const int serverPort = 80;

WiFiClient client;

// Poruka o gresci i beskonacno blinkanje.
void errr(const String &msg)
{
  Serial.println(msg);
  led.blink(0);
}

String sendPhoto()
{
  String getAll;
  String getBody;

  camera_fb_t *fb = cam.getFrameBuffer();
  if (!fb)
    errr("Fejl slikanja");

  Serial.println("Connecting to server: " + serverName);

  //? if (client.connect(serverIP, serverPort))
  if (client.connect(serverName.c_str(), serverPort))
  {
    Serial.println("Connection successful!");
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"image\"; filename=\"espcam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    const int chunkSize = 1024;
    for (size_t n = 0; n < fbLen; n = n + chunkSize)
    {
      if (n + chunkSize < fbLen)
      {
        client.write(fbBuf, chunkSize);
        fbBuf += chunkSize;
      }
      else if (fbLen % chunkSize > 0)
      {
        size_t remainder = fbLen % chunkSize;
        client.write(fbBuf, remainder);
      }
    }
    client.print(tail);

    cam.returnFrameBuffer(fb);

    int timoutTimer = 10000;
    long startTimer = millis();
    bool state = false;

    while ((startTimer + timoutTimer) > millis())
    {
      Serial.print(".");
      delay(100);
      while (client.available())
      {
        char c = client.read();
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
    Serial.println("***********");
    client.stop();
  }
  else
    getBody = "Connection to " + serverName + " failed.";

  Serial.println(getBody);
  return getBody;
}

void setup()
{
  Serial.begin(115200);

  ConnectToWiFi();
  esp_err_t err = cam.setup(framesize_t::FRAMESIZE_SVGA);
  if (err != ESP_OK)
    errr("Greska pri inicijalizaciji kamere.");

  sendPhoto();
}

void loop()
{
  delay(100);
}
