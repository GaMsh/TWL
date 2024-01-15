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
//      if (!NO_INTERNET && !NO_SERVER) {
        delay(10000);
        ESP.restart();
//      } else {
//        previousMillisReboot = currentMillis;
//        Serial.println("But it`s impossible, no internet connection");
//      }
    }
  }
}

//void mainProcess(int currentMillis) {
//  previousMillisPing = currentMillis;
//
//  Serial.println("But it`s impossible, no internet connection");
//
////  if (MODE_SEND_BUFFER) {
////    if (bufferReadAndSend("data")) {
////      MODE_SEND_BUFFER = false;
////    }
////  }
//
//  // BME280
//  float t1;
//  float h1;
//  float p1;
//
//  // HTU21
//  float t2;
//  float h2;
//
//  // SHT31
//  float t3;
//  float h3;
//
//  ////////////////
//
//  // OUTDOOR
//  if (CHIP_TEST) {
//    p1 = 760.25;
//    t1 = 25.29;
//    h1 = 65.93;
//  } else {
//    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
//    BME280::PresUnit presUnit(BME280::PresUnit_Pa);
//    BME280.read(p1, t1, h1, tempUnit, presUnit);
//    p1 = p1 / 133.3224;
//  }
//
//  // INDOOR1
//  if (CHIP_TEST) {
//    t2 = 20.51;
//    h2 = 60.25;
//  } else {
//    t2 = HTU21.readTemperature();
//    h2 = HTU21.readHumidity();
//  }
//
//  // INDOOR2
//  if (CHIP_TEST) {
//    t3 = 20.51;
//    h3 = 60.25;
//  } else {
//    SHT31.UpdateData();
//    t3 = SHT31.GetTemperature();
//    h3 = SHT31.GetRelHumidity();
//  }
//
//  time_t now = time(nullptr);
//
//  boolean someGood = false;
//  String urlString = "token=" + String(TOKEN) + "&";
//  if (!isnan(p1)) {
//    someGood = true;
//    urlString += "t1=" + String(t1) + "&h1=" + String(h1) + "&p1=" + String(p1) + "&";
//    STATUS_BME280_GOOD = true;
//  } else {
//    STATUS_BME280_GOOD = false;
//  }
//  Serial.println(t2);
//  if (t2 != 255 && t2 != 998 && t2 != 999) {
//    someGood = true;
//    urlString += "t2=" + String(t2) + "&" + "h2=" + String(h2) + "&";
//    STATUS_HTU21_GOOD = true;
//  } else {
//    STATUS_HTU21_GOOD = false;
//  }
//  Serial.println(t3);
//  if (t3 != 255 && t3 != 998 && t3 != 999 && h3 != 0) {
//    someGood = true;
//    urlString += "t3=" + String(t3) + "&" + "h3=" + String(h3) + "&";
//    STATUS_SHT31_GOOD = true;
//  } else {
//    STATUS_SHT31_GOOD = false;
//  }
//
//  urlString += "millis=" + String(millis()) + "&" + "time=" + String(time(&now));
//  if (someGood) {
//    actionDo(urlString);
//  }
//}

void checkFirmwareUpdate(bool ignoreConfig) {
  if (ignoreConfig || (!NO_AUTO_UPDATE && !NO_INTERNET && !CHIP_TEST)) {
    WiFiClient wifi;
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    t_httpUpdate_return ret =
        ESPhttpUpdate.update(wifi, FIRMWARE_UPDATE_SERVER, DEVICE_FIRMWARE);
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
    writeLocalBuffer(urlString);
    analogWrite(LED_EXTERNAL, 0);
  }
}

boolean callToServer(String urlString) {
  if (NO_INTERNET) {
    NO_INTERNET = false;
    //bufferReadAndSend("data");
  }

  Serial.println("token=" + String(TOKEN) + "&data=" + urlString);

  WiFiClient wifi;
  HTTPClient http;
  String uri = HTTP_API_SERVER + TOKEN + "/data";
  http.begin(wifi, uri);
  http.setUserAgent(deviceName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("token=" + String(TOKEN) + "&data=" + urlString);
  Serial.println("Sending to server...");
  if (httpCode != HTTP_CODE_OK) {
    NO_SERVER = true;
//    bufferWrite("data", urlString);
    return false;
  }
//  if (NO_SERVER) {
//    NO_SERVER = false;
//    bufferReadAndSend("data");
//  }
  String payload = http.getString();
  Serial.print(String(httpCode) + ": ");
  Serial.println(payload);
  http.end();

  return true;
}

boolean writeLocalBuffer(String urlString) {
  if (!NO_INTERNET) {
    NO_INTERNET = true;
    Serial.println("NO INTERNET MODE ACTIVATED");
  }
  return true; //bufferWrite("data", urlString);
}
