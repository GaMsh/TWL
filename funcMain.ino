void taskConfig(int currentMillis) {
  if (currentMillis - previousMillisConfig > CONFIG_INTERVAL) {
    getDeviceConfiguration(false);
    previousMillisConfig = currentMillis;
  }
}

void taskRestart(int currentMillis) {
  if (!CHIP_TEST) {
    if (currentMillis - previousMillisReboot > REBOOT_INTERVAL) {
      Serial.println("It`s time to reboot");
      delay(10000);
      ESP.restart();
    }
  }
}

void checkFirmwareUpdate(bool ignoreConfig) {
  if (ignoreConfig || (!NO_AUTO_UPDATE && !NO_INTERNET && !CHIP_TEST)) {
    WiFiClient wifi;
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    t_httpUpdate_return ret = ESPhttpUpdate.update(wifi, FIRMWARE_UPDATE_SERVER, DEVICE_FIRMWARE);
    switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n",
                    ESPhttpUpdate.getLastError(),
                    ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[update] Update no Update.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("[update] Update ok.");
      ESP.restart();
      break;
    }
  }
}

boolean sendDataToServer(String json) {
  if (NO_INTERNET) {
    NO_INTERNET = false;
  }
  if (json.length() < 8) {
    return false;
  }

  WiFiClient wifi;
  HTTPClient http;
  String uri = HTTP_API_SERVER + TOKEN + "/data";
  
  http.begin(wifi, uri);
  http.setUserAgent(deviceName);
  http.setTimeout(6000);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(json);
  Serial.println("Отправка данных на сервер..." + json);
  
  if (httpCode != HTTP_CODE_OK) {
    NO_SERVER = true;
    return false;
  }
  String payload = http.getString();
  Serial.print("Результат: " + String(httpCode));
  http.end();

  return true;
}