#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);


const int MAX_SIZE = 10;

// 总配置行数
int configItemLength;

// 输入通道的状态配置
int inputStatus[MAX_SIZE];

// [第几个通道, [每个通道的第几行配置]]
int inputMax[MAX_SIZE][MAX_SIZE] = { 0 };
int inputMin[MAX_SIZE][MAX_SIZE] = { 0 };
int inputIO[MAX_SIZE][MAX_SIZE] = { 0 };

// 输出通道的状态配置
int outputIO[MAX_SIZE][MAX_SIZE] = { 0 };


// 当前实时数据
int nowValue[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int outputPin1 = 20;
int outputPin2 = 21;
int outputPin3 = 47;
int outputPin4 = 48;
int outputPin5 = 45;
int outputPin6 = 40;
int outputPin7 = 41;
int outputPin8 = 42;
int outputPin9 = 18;
int outputPin10 = 17;

int inputPin1 = 1;
int inputPin2 = 2;
int inputPin3 = 3;
int inputPin4 = 4;
int inputPin5 = 5;
int inputPin6 = 6;
int inputPin7 = 7;
int inputPin8 = 8;
int inputPin9 = 9;
int inputPin10 = 10;

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

  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);
  pinMode(inputPin3, INPUT);
  pinMode(inputPin4, INPUT);
  pinMode(inputPin5, INPUT);
  pinMode(inputPin6, INPUT);
  pinMode(inputPin7, INPUT);
  pinMode(inputPin8, INPUT);
  pinMode(inputPin9, INPUT);
  pinMode(inputPin10, INPUT);

  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
  pinMode(outputPin3, OUTPUT);
  pinMode(outputPin4, OUTPUT);
  pinMode(outputPin5, OUTPUT);
  pinMode(outputPin6, OUTPUT);
  pinMode(outputPin7, OUTPUT);
  pinMode(outputPin8, OUTPUT);
  pinMode(outputPin9, OUTPUT);
  pinMode(outputPin10, OUTPUT);


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
    "/setting", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // 将收到的数据解析为 JSON
      JsonDocument jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, data);
      Serial.println("ok");

      if (error) {
        Serial.print("JSON 解析失败: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\", \"status\":\"error\"}");
        return;
      }

      configItemLength = jsonDoc["configItemLength"];

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
          JsonObject item = nowInputListTemp[j1];

          for (JsonPair kv : item) {
            String key = kv.key().c_str();
            int value = kv.value().as<int>();

            if (key == "value") { inputIO[j1][i1] = value; }
            if (key == "max") { inputMax[j1][i1] = value; }
            if (key == "min") { inputMin[j1][i1] = value; }
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

      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      request->send(resp);

      // request->send(200, "application/json", response);
    });



  // server.on("/getValue", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
  //   AsyncWebServerResponse *response = request->beginResponse(200);
  //   response->addHeader("Access-Control-Allow-Origin", "*");
  //   response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  //   response->addHeader("Access-Control-Allow-Headers", "Content-Type");
  //   request->send(response);
  // });

  server.on(
    "/getValue", HTTP_GET, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // 回复客户端
      String response;
      JsonDocument responseDoc;
      responseDoc["status"] = "ok";

      JsonArray arr = responseDoc.createNestedArray("nowValue");
      for (int i = 0; i < 10; i++) {
        arr.add(nowValue[i]);
      }

      serializeJson(responseDoc, response);

      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      request->send(resp);
    });

  server.begin();
}

// 通道转输入pin针
int channelToInputPin(int channel) {
  switch (channel) {
    case 1:
      return inputPin1;
      break;

    case 2:
      return inputPin2;
      break;

    case 3:
      return inputPin3;
      break;

    case 4:
      return inputPin4;
      break;

    case 5:
      return inputPin5;
      break;

    case 6:
      return inputPin6;
      break;

    case 7:
      return inputPin7;
      break;

    case 8:
      return inputPin8;
      break;

    case 9:
      return inputPin9;
      break;

    case 10:
      return inputPin10;
      break;

    default:
      return -1;
  }
}

// 通道转输出pin针
int channelToOutputPin(int channel) {
  switch (channel) {
    case 1:
      return outputPin1;
      break;

    case 2:
      return outputPin2;
      break;

    case 3:
      return outputPin3;
      break;

    case 4:
      return outputPin4;
      break;

    case 5:
      return outputPin5;
      break;

    case 6:
      return outputPin6;
      break;

    case 7:
      return outputPin7;
      break;

    case 8:
      return outputPin8;
      break;

    case 9:
      return outputPin9;
      break;

    case 10:
      return outputPin10;
      break;

    default:
      return -1;
  }
}

// 通道验证
bool channelVerification(int channel, int configStep) {
  int status = inputStatus[channel - 1];

  if (status == -1) {
    nowValue[channel - 1] = 0;
    return true;
  }

  if (status == 0) {
    int max = inputMax[channel - 1][configStep];
    int min = inputMin[channel - 1][configStep];

    int now = analogRead(channelToInputPin(channel));
    nowValue[channel - 1] = now;
    if (now > min && now < max) { return true; }
  }

  if (status == 1) {
    int need = inputIO[channel - 1][configStep];
    int now = digitalRead(channelToInputPin(channel));
    nowValue[channel - 1] = now;
    if (now == need) { return true; }
  }

  return false;
}

void loop() {
  // 测通道的值
  // for (int index = 0; index < configItemLength; index++) {
  //   for (int channel = 1; channel < MAX_SIZE; channel++) {
  //     if (!channelVerification(channel, index)) {
  //       break;
  //     }
  //   }

  //   for (int channel = 1; channel < MAX_SIZE; channel++) {
  //     if (outputIO[index][channel - 1]) {
  //       digitalWrite(channelToOutputPin(channel), HIGH);
  //     } else {
  //       digitalWrite(channelToOutputPin(channel), LOW);
  //     }
  //   }
  // }
  // delay(5000);
}
