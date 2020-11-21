// #include <Arduino.h>
// #define TEST true
// #include <UtilsCommon.h>
// #include <esp_http_server.h>
// #include <esp_camera.h>
// #include <WiFi.h>
// #include <CredWiFi.h>
// #include <SpiffsUtils.h>

// #define PWDN_GPIO_NUM 32
// #define RESET_GPIO_NUM -1
// #define XCLK_GPIO_NUM 0
// #define SIOD_GPIO_NUM 26
// #define SIOC_GPIO_NUM 27

// #define Y9_GPIO_NUM 35
// #define Y8_GPIO_NUM 34
// #define Y7_GPIO_NUM 39
// #define Y6_GPIO_NUM 36
// #define Y5_GPIO_NUM 21
// #define Y4_GPIO_NUM 19
// #define Y3_GPIO_NUM 18
// #define Y2_GPIO_NUM 5
// #define VSYNC_GPIO_NUM 25
// #define HREF_GPIO_NUM 23
// #define PCLK_GPIO_NUM 22

// httpd_handle_t camera_httpd = NULL;

// static esp_err_t index_handler(httpd_req_t *req)
// {
//     Serial.println("index_handler");
//     httpd_resp_set_type(req, "text/html");

//     String s = SpiffsUtils::ReadFile("/index.html");
//     return httpd_resp_send(req, s.c_str(), s.length());
//     //B return httpd_resp_send(req, "<html><body>Test</body></html> ", 31);
// }

// static esp_err_t preview_handler(httpd_req_t *req)
// {
//     camera_fb_t *fb = NULL;
//     esp_err_t res = ESP_OK;
//     int64_t fr_start = esp_timer_get_time();

//     fb = esp_camera_fb_get();
//     if (!fb)
//     {
//         tprintln("Camera capture failed");
//         httpd_resp_send_500(req);
//         return ESP_FAIL;
//     }

//     httpd_resp_set_type(req, "image/jpeg");
//     httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
//     httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

//     // size_t out_len, out_width, out_height;
//     // uint8_t *out_buf;
//     // bool s;
//     // bool detected = false;
//     // int face_id = 0;

//     size_t fb_len = 0;
//     // if (fb->format == PIXFORMAT_JPEG)
//     fb_len = fb->len;
//     res = httpd_resp_send(req, (const char *)fb->buf, fb->len);

//     esp_camera_fb_return(fb);
//     int64_t fr_end = esp_timer_get_time();
//     Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
//     return res;
// }

// void startCameraServer()
// {
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();

//     httpd_uri_t index_uri = {
//         .uri = "/",
//         .method = HTTP_GET,
//         .handler = index_handler,
//         .user_ctx = NULL};

//     // httpd_uri_t cmd_uri = {
//     //     .uri = "/control",
//     //     .method = HTTP_GET,
//     //     .handler = cmd_handler,
//     //     .user_ctx = NULL};

//     httpd_uri_t preview_uri = {
//         .uri = "/preview",
//         .method = HTTP_GET,
//         .handler = preview_handler,
//         .user_ctx = NULL};

//     // ra_filter_init(&ra_filter, 20);

//     // Serial.printf("Starting web server on port: '%d'\n", config.server_port);
//     if (httpd_start(&camera_httpd, &config) == ESP_OK)
//     {
//         httpd_register_uri_handler(camera_httpd, &index_uri);
//         // httpd_register_uri_handler(camera_httpd, &cmd_uri);
//         httpd_register_uri_handler(camera_httpd, &preview_uri);
//     }
// }

// void setup()
// {
//     tbegin(115200);
//     SPIFFS.begin();

//     camera_config_t config;
//     config.ledc_channel = LEDC_CHANNEL_0;
//     config.ledc_timer = LEDC_TIMER_0;
//     config.pin_d0 = Y2_GPIO_NUM;
//     config.pin_d1 = Y3_GPIO_NUM;
//     config.pin_d2 = Y4_GPIO_NUM;
//     config.pin_d3 = Y5_GPIO_NUM;
//     config.pin_d4 = Y6_GPIO_NUM;
//     config.pin_d5 = Y7_GPIO_NUM;
//     config.pin_d6 = Y8_GPIO_NUM;
//     config.pin_d7 = Y9_GPIO_NUM;
//     config.pin_xclk = XCLK_GPIO_NUM;
//     config.pin_pclk = PCLK_GPIO_NUM;
//     config.pin_vsync = VSYNC_GPIO_NUM;
//     config.pin_href = HREF_GPIO_NUM;
//     config.pin_sscb_sda = SIOD_GPIO_NUM;
//     config.pin_sscb_scl = SIOC_GPIO_NUM;
//     config.pin_pwdn = PWDN_GPIO_NUM;
//     config.pin_reset = RESET_GPIO_NUM;
//     config.xclk_freq_hz = 20000000;
//     config.pixel_format = PIXFORMAT_JPEG;

//     // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
//     //                      for larger pre-allocated frame buffer.
//     Serial.print("PSRAM: ");
//     if (psramFound())
//     {
//         // config.frame_size = FRAMESIZE_UXGA;
//         config.jpeg_quality = 10;
//         config.fb_count = 2;
//         Serial.println("OK");
//     }
//     else
//     {
//         // config.frame_size = FRAMESIZE_SVGA;
//         config.jpeg_quality = 12;
//         config.fb_count = 1;
//         Serial.println("NOT OK");
//     }
//     config.frame_size = FRAMESIZE_QVGA;

//     // camera init
//     esp_err_t err = esp_camera_init(&config);
//     if (err != ESP_OK)
//     {
//         Serial.printf("Camera init failed with error 0x%x", err);
//         return;
//     }
//     Serial.println("Camera init success!");

//     sensor_t *s = esp_camera_sensor_get();
//     // drop down frame size for higher initial frame rate
//     s->set_framesize(s, FRAMESIZE_QVGA);

//     Serial.print("WiFi connecting...");
//     WiFi.begin(WIFI_SSID, WIFI_PASS);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println();
//     Serial.print("WiFi connected: ");
//     IPAddress ipa(192, 168, 0, 60);
//     IPAddress gateway(192, 168, 0, 254);
//     IPAddress subnet(255, 255, 255, 0);
//     WiFi.config(ipa, gateway, subnet);
//     Serial.println(WiFi.localIP());

//     startCameraServer();
// }

// void loop()
// {
//     delay(10000);
// }
