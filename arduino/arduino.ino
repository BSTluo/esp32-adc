#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);

int input1Status = -1;
int input2Status = -1;
int input3Status = -1;
int input4Status = -1;
int input5Status = -1;
int input6Status = -1;
int input7Status = -1;
int input8Status = -1;
int input9Status = -1;
int input10Status = -1;

int intput1Max = 0;
int intput2Max = 0;
int intput3Max = 0;
int intput4Max = 0;
int intput5Max = 0;
int intput6Max = 0;
int intput7Max = 0;
int intput8Max = 0;
int intput9Max = 0;
int intput10Max = 0;

int intput1Min = 0;
int intput2Min = 0;
int intput3Min = 0;
int intput4Min = 0;
int intput5Min = 0;
int intput6Min = 0;
int intput7Min = 0;
int intput8Min = 0;
int intput9Min = 0;
int intput10Min = 0;

int input1IO = 0;
int input2IO = 0;
int input3IO = 0;
int input4IO = 0;
int input5IO = 0;
int input6IO = 0;
int input7IO = 0;
int input8IO = 0;
int input9IO = 0;
int input10IO = 0;

int output1Setting = 0;
int output2Setting = 0;
int output3Setting = 0;
int output4Setting = 0;
int output5Setting = 0;
int output6Setting = 0;
int output7Setting = 0;
int output8Setting = 0;
int output9Setting = 0;
int output10Setting = 0;

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
    "/setting", HTTP_POST, [](AsyncWebServerRequest *request) {},
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
      JsonArray inputList = jsonDoc["inputList"];
      JsonArray nowInputList = jsonDoc["nowInputList"];

      JsonArray outputList = jsonDoc["outputList"];
      JsonArray nowOutputList = jsonDoc["nowOutputList"];

      for (JsonObject inputListTemp : inputList) {
        int name = inputListTemp["name"];
        int status = inputListTemp["status"];
        
      }

      // 回复客户端
      String response;
      JsonDocument responseDoc;
      responseDoc["status"] = "received";
      responseDoc["name"] = "ok";
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
