//* Odvaja se posebna memorija za payload i u nju se kopiraju zaglavlje, slika(fajl) i rep.
//* (Valjda) radi za manje slike.

#include <WiFi.h>
#include <CredWiFi.h>
#include <HTTPClient.h>

#include <AiThinkerCam.h>
AiThinkerCam cam;

void FreeMemKB(const char *caption) { Serial.printf("%s\t%d\n", caption, ESP.getFreeHeap() / 1024); }

int itvMain;

// Poruka o gresci i beskonacno blinkanje.
void errr(const String &msg)
{
    Serial.println(msg);
    while (1)
        delay(100);
}

void setup()
{
    Serial.begin(115200);
    FreeMemKB("esp setup");
    //B delay(4000);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.println("\nConnected to the WiFi network");

    FreeMemKB("wifi setup");
    esp_err_t err = cam.setup(framesize_t::FRAMESIZE_QVGA);
    // Ovo je max rezolucija ako se slika kopira zbog slanja. Na vecim rezolucijama ESP kasnije puca.
    if (err != ESP_OK)
        errr("Greska pri inicijalizaciji kamere.");
    FreeMemKB("cam setup");

    HTTPClient http;
    http.begin("https://imageuploadbv.azurewebsites.net/api/ImgUpload/getVersion");
    Serial.println(http.connect());

    client.println("GET https://imageuploadbv.azurewebsites.net/api/ImgUpload/getVersion HTTP/1.1");
    ulong timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            delay(5000);
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    while (1)
        delay(100);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        camera_fb_t *fb = cam.getFrameBuffer();
        if (!fb)
            errr("Fejl slikanja");

        HTTPClient http;
        http.begin("https://imageuploadbv.azurewebsites.net/api/ImgUpload");
        http.addHeader("Content-Type", "multipart/form-data;boundary=\"boundary\"");

        String head = "--boundary\r\n"
                      "Content-Disposition: form-data; name=\"Name\"\r\n\r\n"
                      "test.jpg\r\n"
                      "--boundary\r\n"
                      "Content-Disposition: form-data; name=\"File\"; filename=\"test.jpg\"\r\n\r\n";
        const char *tail = "\r\n--boundary--\r\n\0";
        //T const char *content = "PetiKuli\r\nZeka!";
        size_t len = head.length() + fb->len + strlen(tail);
        //T size_t len = head.length() + strlen(content) + strlen(tail);
        // Serial.println(len);
        uint8_t *b = (uint8_t *)malloc(len /*+1*/);
        // Serial.printf("b alloc %p\n", b);
        FreeMemKB("mem alloc");

        memcpy(b, head.c_str(), head.length());
        size_t pos = head.length();
        memcpy(b + pos, fb->buf, fb->len);
        pos += fb->len;
        //T memcpy(b + pos, content, strlen(content));
        //T pos += strlen(content);
        memcpy(b + pos, tail, strlen(tail));
        //T b[len] = 0;
        //T Serial.println((char*)b);
        //T while (1)
        //T     delay(100);

        // http.writeToStream
        Serial.println("POST BEGIN");
        int httpResponseCode = http.POST(b, len);
        Serial.println("POST END");
        cam.returnFrameBuffer(fb);
        FreeMemKB("free pic");
        free(b);
        FreeMemKB("free bytes");
        if (httpResponseCode > 0)
        {
            String response = http.getString(); //Get the response to the request
            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer
            itvMain = 15000;
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
            itvMain = 5000;
        }
        http.end(); //Free resources
    }
    else
        Serial.println("Error in WiFi connection");

    delay(itvMain);
}
