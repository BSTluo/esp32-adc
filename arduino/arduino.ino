#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(9600);

  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);

  WiFi.softAP("Analog", "kiekert123");

  while (WiFi.softAPgetStationNum() <= 0) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // webServer.on("/", handleRoot);                          //设置主页回调函数
  // webServer.on("/assets/index-CdHVMU6r.js", handleJs);    //设置主页回调函数
  // webServer.on("/assets/index-BdrH_tCX.css", handleCss);  //设置主页回调函数

  server.on(
    "/json", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // 将收到的数据解析为 JSON
      JsonDocument jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, data);

      if (error) {
        Serial.print("JSON 解析失败: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      // 读取字段，比如 name 和 age
      const char *name = jsonDoc["name"];
      int age = jsonDoc["age"];

      Serial.printf("收到 JSON：name = %s, age = %d\n", name, age);

      // 回复客户端
      String response;
      JsonDocument responseDoc;
      responseDoc["status"] = "received";
      responseDoc["name"] = name;
      serializeJson(responseDoc, response);

      request->send(200, "application/json", response);
    });

  server.begin();
}

void loop() {
  // 测通道的值
  // int analogValue1 = analogRead(1);
  // int analogValue2 = analogRead(2);
  // int analogValue3 = analogRead(3);
  // int analogValue4 = analogRead(4);
  // int analogValue5 = analogRead(5);
  // int analogValue6 = analogRead(6);

  // Serial.printf("ADC analog value = %d\n", analogValue);
  // Serial.printf("ADC millivolts value = %d\n", analogVolts);

  // delay(1000);
}
