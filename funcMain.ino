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

void actionDo(String urlString) {
  if (WiFi.status() == WL_CONNECTED) {
    analogWrite(LED_EXTERNAL, 0);
    callToServer(urlString);
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
  } else {
    analogWrite(LED_EXTERNAL, LED_BRIGHT);
//    writeLocalBuffer(urlString);
    analogWrite(LED_EXTERNAL, 0);
  }
}

boolean callToServer(String urlString) {
  if (NO_INTERNET) {
    NO_INTERNET = false;
  }

//   Serial.println("token=" + String(TOKEN) + "&data=" + urlString);

  WiFiClient wifi;
  HTTPClient http;
  String uri = HTTP_API_SERVER + TOKEN + "/data";
  
  http.begin(wifi, uri);
  http.setUserAgent(deviceName);
  http.setTimeout(6000);
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["data"] = urlString;

  String json;
  serializeJson(doc, json);

  int httpCode = http.POST(json);
  Serial.println("Sending to server...");
  
  if (httpCode != HTTP_CODE_OK) {
    NO_SERVER = true;
    return false;
  }
  String payload = http.getString();
  Serial.print(String(httpCode) + ": ");
  Serial.println(payload);
  http.end();

  return true;
}

//boolean writeLocalBuffer(String urlString) {
//  if (!NO_INTERNET) {
//    NO_INTERNET = true;
//    Serial.println("NO INTERNET MODE ACTIVATED");
//  }
//  return true;
//}
