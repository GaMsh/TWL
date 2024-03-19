void loop() {
  unsigned long currentMillis = millis();

  taskConfig(currentMillis);
  taskRestart(currentMillis);

  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, 33);
  } else {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  }
  if (SENS_INTERVAL > 0) {
    if (currentMillis - previousMillis_SENS >= SENS_INTERVAL) {
      String json;
      JsonDocument toSend;
      previousMillis_SENS = currentMillis;
      JsonDocument BME280 = getSensorData_BME280(currentMillis);
      if (!BME280.isNull()) {
        toSend["data"]["BME280"] = BME280;
      }
      JsonDocument HTU21 = getSensorData_HTU21(currentMillis);
      if (!HTU21.isNull()) {
        toSend["data"]["HTU21"] = HTU21;
      }
      JsonDocument SHT31 = getSensorData_SHT31(currentMillis);
      if (!SHT31.isNull()) {
        toSend["data"]["SHT31"] = SHT31;
      }
      serializeJson(toSend, json);
      sendDataToServer(json);
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  } else {
    analogWrite(LED_EXTERNAL, 33);
  }

  if (MONITOR_SLUG && MONITOR_INTERVAL > 0 && currentMillis - previousMillis_MONITOR >= MONITOR_INTERVAL) {
    Serial.println("Get data from monitor: " + MONITOR_SLUG);
    previousMillis_MONITOR = currentMillis;

    WiFiClient wifi;
    HTTPClient http;
    String uri = HTTP_API_SERVER + MONITOR_SLUG + "/data";
    http.begin(wifi, uri);
    http.setUserAgent(deviceName);
    http.setTimeout(6000);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      NO_SERVER = false;

      String json = http.getString();
      Serial.println(json);
      JsonDocument doc;
      deserializeJson(doc, json);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(MONITOR_NAME);
      display.println(doc["data"][0].as<String>());
      display.println(doc["data"][1].as<String>());
      display.println(doc["data"][2].as<String>());
      display.display();
    }

    http.end();
  }

  if (STATE_INTERVAL > 0 && currentMillis - previousMillis_STATE >= STATE_INTERVAL) {
    Serial.println("Check state");
    previousMillis_STATE = currentMillis;

    WiFiClient wifi;
    HTTPClient http;
    String uri = HTTP_API_SERVER + TOKEN + "/state";
    http.begin(wifi, uri);
    http.setUserAgent(deviceName);
    http.setTimeout(6000);

    int httpCode = http.GET();
    Serial.println("HTTP Code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
      NO_SERVER = false;
    } else {
      return;
    }

    String json = http.getString();
    JsonDocument doc;
    deserializeJson(doc, json);
    http.end();

    String state1 = doc["state1"].as<String>();
    String state2 = doc["state2"].as<String>();
    String state3 = doc["state3"].as<String>();
    String state4 = doc["state4"].as<String>();
    String state5 = doc["state5"].as<String>();
    String state6 = doc["state6"].as<String>();
    String state7 = doc["state7"].as<String>();
    String state8 = doc["state8"].as<String>();
//     digitalWrite(LED_BUILTIN, !state5);
//     digitalWrite(D0, state6);
//     setLedLen(state7, state8);
  }
}
