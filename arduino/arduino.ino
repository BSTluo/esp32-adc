#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);


const int MAX_SIZE = 10;

int inputStatus[MAX_SIZE];

int inputMax[MAX_SIZE][MAX_SIZE] = { 0 };
int inputMin[MAX_SIZE][MAX_SIZE] = { 0 };
int inputIO[MAX_SIZE][MAX_SIZE] = { 0 };
int outputIO[MAX_SIZE][MAX_SIZE] = { 0 };


void print2DArray(int arr[MAX_SIZE][MAX_SIZE]) {
  for (int i = 0; i < MAX_SIZE; i++) {
    for (int j = 0; j < MAX_SIZE; j++) {
      Serial.print(arr[i][j]);
      Serial.print("\t");  // 用制表符分隔
    }
    Serial.println();  // 换行
  }
}

void print1DArray(int arr[MAX_SIZE]) {
  for (int i = 0; i < MAX_SIZE; i++) {
    Serial.print(arr[i]);
    Serial.print("\t");  // 可选：让数据更整齐
  }
  Serial.println();  // 打印完一行后换行
}

void setup() {

  for (int i = 0; i < MAX_SIZE; i++) {
    inputStatus[i] = -1;
  }

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

  server.on("/setting", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  server.on(
    "/setting", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // 将收到的数据解析为 JSON
      JsonDocument jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, data);
      Serial.println("ok");

      if (error) {
        Serial.print("JSON 解析失败: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      // 读取字段，比如 name 和 age
      JsonArray inputList = jsonDoc["inputList"];
      JsonArray nowInputList = jsonDoc["nowInputList"];

      JsonArray nowOutputList = jsonDoc["nowOutputList"];

      for (JsonDocument inputListTemp : inputList) {
        int name = inputListTemp["name"];
        int status = inputListTemp["status"];

        inputStatus[name - 1] = status;
      }

      for (int i1 = 0; i1 < nowInputList.size(); ++i1) {
        JsonArray nowInputListTemp = nowInputList[i1];

        for (int j1 = 0; j1 < nowInputListTemp.size(); ++j1) {
          JsonDocument item = nowInputListTemp[j1];

          String aaa;
          DeserializationError error2 = deserializeJson(item, aaa);
          if (error2) {
            Serial.print("JSON 解析失败: ");
            Serial.println(error.c_str());
            request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
          }
          Serial.println(aaa);

          if (item["value"].is<int>()) {
            inputIO[j1][i1] = item["value"].as<int>();
          }

          if (item["max"].is<int>() && item["min"].is<int>()) {
            inputMax[j1][i1] = item["max"].as<int>();
            inputMin[j1][i1] = item["min"].as<int>();
          }
        }
      }

      for (int i2 = 0; i2 < nowOutputList.size(); ++i2) {
        JsonArray nowOutputListTemp = nowOutputList[i2];

        for (int j2 = 0; j2 < nowOutputListTemp.size(); ++j2) {
          int item = nowOutputListTemp[j2];
          outputIO[j2][i2] = item;
        }
      }

      // 回复客户端
      String response;
      JsonDocument responseDoc;
      responseDoc["status"] = "ok";
      serializeJson(responseDoc, response);

      Serial.println("inputMax:");
      print2DArray(inputMax);

      Serial.println("inputMin:");
      print2DArray(inputMin);

      Serial.println("inputIO:");
      print2DArray(inputIO);

      Serial.println("outputIO:");
      print2DArray(outputIO);

      Serial.println("inputStatus:");
      print1DArray(inputStatus);

      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      request->send(resp);

      // request->send(200, "application/json", response);
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
