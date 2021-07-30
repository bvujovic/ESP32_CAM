//* Na osnovu: https://techtutorialsx.com/2017/05/20/esp32-http-post-requests/
//* Slanje POST zahteva ka serveru. Testirane varijante: plain text, form url-encoded
//*   i multipart/form-data.

#include <WiFi.h>
#include <CredWiFi.h>
#include <HTTPClient.h>

void setup()
{
    Serial.begin(115200);
    // delay(1000); //Delay needed before calling the WiFi.begin
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.println("\nConnected to the WiFi network");
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin("https://imageuploadbv.azurewebsites.net/api/ImgUpload");

        //* plain text
        // http.addHeader("Content-Type", "text/plain"); //Specify content-type header
        // int httpResponseCode = http.POST("POSTING from ESP32");

        //* form url-encoded
        // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // int httpResponseCode = http.POST("test=BME280");

        http.addHeader("Content-Type", "multipart/form-data;boundary=\"boundary\"");

        String payload = R"===(--boundary
Content-Disposition: form-data; name="Name"

test.txt
--boundary
Content-Disposition: form-data; name="File"; filename="example.txt"

Glupog
li
teksta.
--boundary--
)===";
        payload.replace("\n", "\r\n"); // multiline tekst od gore sadrzi samo \n (bez \r ispred)
        // pa se dobija greska 400 - linija prelazi maksimalnih 100 karaktera

        int httpResponseCode = http.POST((uint8_t *)payload.c_str(), payload.length());
        if (httpResponseCode > 0)
        {
            String response = http.getString(); //Get the response to the request
            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end(); //Free resources
    }
    else
        Serial.println("Error in WiFi connection");

    delay(30000);
}
