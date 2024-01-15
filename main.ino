void loop() {
  unsigned long currentMillis = millis();

  taskConfig(currentMillis);
  taskRestart(currentMillis);

  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, 25);
  } else {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  }
  if (currentMillis - previousMillis_SENS_OUTDOOR >= SENS_INTERVAL) {
    Serial.println("SL");
    previousMillis_SENS_OUTDOOR = currentMillis;
    sendSensorData_OUTDOOR(currentMillis);
  }
  if (currentMillis - previousMillis_SENS_INDOOR1 >= SENS_INTERVAL) {
    previousMillis_SENS_INDOOR1 = currentMillis;
    sendSensorData_INDOOR1(currentMillis);
  }
  if (currentMillis - previousMillis_SENS_INDOOR2 >= SENS_INTERVAL) {
    previousMillis_SENS_INDOOR2 = currentMillis;
    sendSensorData_INDOOR2(currentMillis);
  }
  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  } else {
    analogWrite(LED_EXTERNAL, 25);
  }

  if (MONITOR_INTERVAL > 0 &&
      currentMillis - previousMillis_MONITOR >= MONITOR_INTERVAL) {
    Serial.println("Check monitor");
    previousMillis_MONITOR = currentMillis;

    WiFiClient wifi;
    HTTPClient http;
    String uri = HTTP_API_SERVER + TARGET_TOKEN + "/data";
    http.begin(wifi, uri);
    http.setUserAgent(deviceName);
    http.setTimeout(5000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      NO_SERVER = false;

      String payload = http.getString();
      Serial.println(payload);
      StaticJsonDocument<2048> doc;
      deserializeJson(doc, payload, DeserializationOption::NestingLimit(6));
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Ligovka");
      display.println(doc["data"][1].as<String>());
      display.println(doc["data"][2].as<String>());
      display.println(doc["data"][3].as<String>());
      //    display.print(doc["data"]["summary"]["paramsRaw"]["T:"].as<float>());
      display.display();
    }

    http.end();
  }

  if (currentMillis - previousMillis_STATE >= STATE_INTERVAL) {
    Serial.println("Check state");
    previousMillis_STATE = currentMillis;

    WiFiClient wifi;
    HTTPClient http;
    String uri = HTTP_API_SERVER + TOKEN + "/state";
    http.begin(wifi, uri);
    http.setUserAgent(deviceName);
    http.setTimeout(5000);

    int httpCode = http.GET();
    Serial.println("HTTP Code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
      NO_SERVER = false;
    } else {
      return; // false;
    }

    String payload = http.getString();

    const size_t capacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(8) + 256;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, payload);

    http.end();

    int state1 = doc["state1"].as<int>();
    int state2 = doc["state2"].as<int>();
    int state3 = doc["state3"].as<int>();
    int state4 = doc["state4"].as<int>();
    int state5 = doc["state5"].as<int>();
    int state6 = doc["state6"].as<int>();
    int state7 = doc["state7"].as<int>();
    int state8 = doc["state8"].as<int>();
    digitalWrite(LED_BUILTIN, !state5);
    digitalWrite(D0, state1);
    setLedLen(state3, state2);
  }

  //    if (currentMillis - previousMillisReport >= REPORT_INTERVAL) {
  //        previousMillisReport = currentMillis;
  //        STATUS_REPORT_SEND = false;
  //    }
  //
  //    if (!STATUS_REPORT_SEND) {
  //        if (!STATUS_BME280_GOOD) {
  ////            callServer("E", "", "OUTDOOR");
  //        }
  //        if (!STATUS_HTU21_GOOD) {
  ////            callServer("E", "", "INDOOR1");
  //        }
  //        if (!STATUS_SHT31_GOOD) {
  ////            callServer("E", "", "INDOOR2");
  //        }
  //        STATUS_REPORT_SEND = true;
  //    }
}
