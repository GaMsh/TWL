void loop() {
  unsigned long currentMillis = millis();

  taskConfig(currentMillis);
  taskRestart(currentMillis);

  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, 25);
  } else {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  }
  if (SENS_INTERVAL > 0) {
    if (currentMillis - previousMillis_SENS_BME280 >= SENS_INTERVAL) {
      Serial.println("Sensor read: BME280");
      previousMillis_SENS_BME280 = currentMillis;
      sendSensorData_BME280(currentMillis);
    }
    if (currentMillis - previousMillis_SENS_HTU21 >= SENS_INTERVAL) {
      Serial.println("Sensor read: HTU21");
      previousMillis_SENS_HTU21 = currentMillis;
      sendSensorData_HTU21(currentMillis);
    }
    if (currentMillis - previousMillis_SENS_SHT31 >= SENS_INTERVAL) {
      Serial.println("Sensor read: SHT31");
      previousMillis_SENS_SHT31 = currentMillis;
      sendSensorData_SHT31(currentMillis);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  } else {
    analogWrite(LED_EXTERNAL, 25);
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
//       display.print(doc["data"]["summary"]["paramsRaw"]["T:"].as<float>());
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
    http.setTimeout(5000);

    int httpCode = http.GET();
    Serial.println("HTTP Code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
      NO_SERVER = false;
    } else {
      return; // false;
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

  //    if (currentMillis - previousMillisReport >= REPORT_INTERVAL) {
  //        previousMillisReport = currentMillis;
  //        STATUS_REPORT_SEND = false;
  //    }
  //
  //    if (!STATUS_REPORT_SEND) {
  //        if (!STATUS_BME280_GOOD) {
  ////            callServer("E", "", "BME280");
  //        }
  //        if (!STATUS_HTU21_GOOD) {
  ////            callServer("E", "", "HTU21");
  //        }
  //        if (!STATUS_SHT31_GOOD) {
  ////            callServer("E", "", "SHT31");
  //        }
  //        STATUS_REPORT_SEND = true;
  //    }
}
