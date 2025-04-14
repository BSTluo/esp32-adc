void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(9600);

  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);
}

void loop() {
  // 测通道的值
  int analogValue1 = analogRead(1);
  int analogValue2 = analogRead(2);
  int analogValue3 = analogRead(3);
  int analogValue4 = analogRead(4);
  int analogValue5 = analogRead(5);
  int analogValue6 = analogRead(6);

  // Serial.printf("ADC analog value = %d\n", analogValue);
  // Serial.printf("ADC millivolts value = %d\n", analogVolts);

  delay(1000);  // delay in between reads for clear read from serial
}
