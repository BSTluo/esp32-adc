#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <FS.h>

AsyncWebServer server(80);
Preferences prefs;

// 当前滤波数量
#define FilterCount 10

// 最大通道数
#define MAX_SIZE 10

// 总配置行数
int configItemLength = 0;

// 输入通道的状态配置
int inputStatus[MAX_SIZE];

// [第几个通道, [每个通道的第几行配置]]
int inputMax[MAX_SIZE][MAX_SIZE] = { 0 };
int inputMin[MAX_SIZE][MAX_SIZE] = { 0 };
int inputIO[MAX_SIZE][MAX_SIZE] = { 0 };

// 输出通道的状态配置
int outputIO[MAX_SIZE][MAX_SIZE] = { 0 };


// 当前实时数据
int nowInputValue[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int nowOutputValue[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int *nowOutputValueArr[MAX_SIZE];

// 当前输出的针脚
int outputPin[] = { 20, 21, 35, 47, 45, 40, 41, 42, 18, 17 };

// 当前输入的针脚
int inputPin[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

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

void saveConfig() {
  prefs.begin("config", false);

  prefs.putInt("cfg_len", configItemLength);
  prefs.putBytes("in_status", inputStatus, sizeof(inputStatus));
  prefs.putBytes("in_max", inputMax, sizeof(inputMax));
  prefs.putBytes("in_min", inputMin, sizeof(inputMin));
  prefs.putBytes("in_io", inputIO, sizeof(inputIO));
  prefs.putBytes("out_io", outputIO, sizeof(outputIO));

  prefs.end();
}

void loadConfig() {
  prefs.begin("config", true);  // 只读模式

  if (prefs.isKey("cfg_len")) {
    configItemLength = prefs.getInt("cfg_len");
  }

  if (prefs.isKey("in_status")) {
    prefs.getBytes("in_status", inputStatus, sizeof(inputStatus));
  }

  if (prefs.isKey("in_max")) {
    prefs.getBytes("in_max", inputMax, sizeof(inputMax));
  }

  if (prefs.isKey("in_min")) {
    prefs.getBytes("in_min", inputMin, sizeof(inputMin));
  }

  if (prefs.isKey("in_io")) {
    prefs.getBytes("in_io", inputIO, sizeof(inputIO));
  }

  if (prefs.isKey("out_io")) {
    prefs.getBytes("out_io", outputIO, sizeof(outputIO));
  }

  createOutputArrObject();
  prefs.end();
}

void createOutputArrObject() {
  for (int i = 0; i < MAX_SIZE; i++) {

    if (configItemLength > 0) {
      nowOutputValueArr[i] = (int *)malloc(configItemLength * sizeof(int));

      for (int j = 0; j < configItemLength; j++) {
        nowOutputValueArr[i][j] = 0;
      }
    } else {
      nowOutputValueArr[i] = nullptr;  // 或者保留 NULL 以防误访问
    }
  }
}

void setup() {
  for (int i = 0; i < MAX_SIZE; i++) {
    inputStatus[i] = -1;
  }

  pinMode(inputPin[0], INPUT);
  pinMode(inputPin[1], INPUT);
  pinMode(inputPin[2], INPUT);
  pinMode(inputPin[3], INPUT);
  pinMode(inputPin[4], INPUT);
  pinMode(inputPin[5], INPUT);
  pinMode(inputPin[6], INPUT);
  pinMode(inputPin[7], INPUT);
  pinMode(inputPin[8], INPUT);
  pinMode(inputPin[9], INPUT);

  pinMode(outputPin[0], OUTPUT);
  pinMode(outputPin[1], OUTPUT);
  pinMode(outputPin[2], OUTPUT);
  pinMode(outputPin[3], OUTPUT);
  pinMode(outputPin[4], OUTPUT);
  pinMode(outputPin[5], OUTPUT);
  pinMode(outputPin[6], OUTPUT);
  pinMode(outputPin[7], OUTPUT);
  pinMode(outputPin[8], OUTPUT);
  pinMode(outputPin[9], OUTPUT);

  loadConfig();

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }

  // initialize serial communication at 115200 bits per second:
  Serial.begin(9600);

  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);

  WiFi.softAP("Analog", "kiekert123");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

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

      saveConfig();
      createOutputArrObject();

      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      request->send(resp);

      // request->send(200, "application/json", response);
    });

  server.on("/getValue", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response;
    JsonDocument responseDoc;
    responseDoc["status"] = "ok";

    JsonArray arr1 = responseDoc.createNestedArray("nowInputValue");
    for (int i = 0; i < 10; i++) {
      arr1.add(nowInputValue[i]);
    }

    JsonArray arr2 = responseDoc.createNestedArray("nowOutputValue");
    for (int i = 0; i < 10; i++) {
      arr2.add(nowOutputValue[i]);
    }

    serializeJson(responseDoc, response);

    // 设置允许跨域访问的头部
    AsyncWebServerResponse *res = request->beginResponse(200, "text/plain", response);
    res->addHeader("Access-Control-Allow-Origin", "*");  // 允许所有来源访问
    request->send(res);
  });

  server.begin();
}

// 通道转输入pin针
int channelToInputPin(int channel) {
  return inputPin[channel - 1];
}

// 通道转输出pin针
int channelToOutputPin(int channel) {
  return outputPin[channel - 1];
}

// 简易滤波
int simpleAverageFilter(int channel, int samples = 10) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(channelToInputPin(channel));
    delayMicroseconds(1);
  }
  return sum / samples;
}

// 通道验证
bool channelVerification(int channel, int configStep) {
  int status = inputStatus[channel - 1];

  if (status == -1) {
    nowInputValue[channel - 1] = -1;
    return true;
  }

  if (status == 0) {
    int max = inputMax[channel - 1][configStep];
    int min = inputMin[channel - 1][configStep];

    int now = simpleAverageFilter(channel, FilterCount);
    nowInputValue[channel - 1] = now;

    if (now >= min && now <= max) { return true; }
  }

  if (status == 1) {
    int need = inputIO[channel - 1][configStep];
    int now = digitalRead(channelToInputPin(channel));
    nowInputValue[channel - 1] = now;
    if (now == need) { return true; }
  }

  return false;
}

void loop() {
  // 测通道的值

  for (int index = 0; index < configItemLength; index++) {
    int skip = 0;
    for (int channel = 1; channel <= MAX_SIZE; channel++) {
      if (!channelVerification(channel, index)) {
        skip = 1;
      }
    }

    for (int channel = 1; channel <= MAX_SIZE; channel++) {

      if (outputIO[channel - 1][index] == 0) {
        nowOutputValueArr[channel - 1][index] = 0;
      }

      if (skip == 1) {
        if (outputIO[channel - 1][index] == 1) {
          nowOutputValueArr[channel - 1][index] = 0;
        }
      } else {
        if (outputIO[channel - 1][index] == 1) {
          nowOutputValueArr[channel - 1][index] = 1;
        }
      }
    }
  }

  for (int channel = 0; channel < MAX_SIZE; channel++) {
    int sumNumber = 0;
    for (int index = 0; index < configItemLength; index++) {
      sumNumber = sumNumber + nowOutputValueArr[channel][index];
    }

    if (sumNumber > 0) {
      nowOutputValue[channel] = 1;
      digitalWrite(channelToOutputPin(channel + 1), HIGH);
    } else {
      nowOutputValue[channel] = 0;
      digitalWrite(channelToOutputPin(channel + 1), LOW);
    }
  }
}
