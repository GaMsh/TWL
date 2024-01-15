// core functions
bool getDeviceConfiguration(bool first) {
  StaticJsonDocument<1024> jb;
  String postData =
      "device=" + getSensorID() + "&" + "token=" + TOKEN + "&" +
      "revision=" + String(DEVICE_REVISION) + "&model=" + String(DEVICE_MODEL) + "&" +
      "firmware=" + String(DEVICE_FIRMWARE) + "&ip=" + WiFi.localIP().toString() + "&" +
      "mac=" + String(WiFi.macAddress()) + "&" + "ssid=" + String(WiFi.SSID()) + "&" +
      "rssi=" + String(WiFi.RSSI()) + "&" + "vcc=" + String(ESP.getVcc()) +
      "&" + "nu=" + String(NO_AUTO_UPDATE) + "&" + "ct=" + String(CHIP_TEST);
  Serial.println(postData);

  const size_t capacity = JSON_OBJECT_SIZE(10) + JSON_ARRAY_SIZE(10) + 256;
  DynamicJsonDocument doc(capacity);

  WiFiClient wifi;
  HTTPClient http;
  http.begin(wifi, String(HTTP_API_SERVER) + "reg");
  http.setUserAgent(deviceName);
  http.setTimeout(30000);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);
  if (!first && httpCode < 0) {
    NO_SERVER = true;
    return false;
  } else {
    if (httpCode != HTTP_CODE_OK && !CHIP_TEST) {
      ticker1.attach_ms(200, tickInternal);
      ticker2.attach_ms(500, tickExternal, MAIN_MODE_OFFLINE);
      Serial.println("Failed to initialize the device using the server");
      Serial.println(httpCode);
      delay(15000);
      ESP.restart();
      return false;
    }
  }
  Serial.println("Get device configuration from server, HTTP Code: " + String(httpCode));
  if (httpCode == HTTP_CODE_OK) {
    NO_SERVER = false;
  } else {
    return false;
  }

  String payload = http.getString();
  deserializeJson(doc, payload);
  http.end();

  int serverTime = doc["time"].as<int>();
  Serial.println(serverTime);

  struct timeval tv;
  tv.tv_sec = serverTime;
  settimeofday(&tv, NULL); // Применяем время сервера, как локальное

  if (TOKEN != doc["token"].as<String>()) {
    TOKEN = doc["token"].as<String>();
    writeCfgFile("token", TOKEN);
    Serial.println("Device token was updated in store");
  }

  if (doc["state1"].as<int>() > 0) {
    int LED_BRIGHT_NEW = doc["state1"].as<int>();
    if (LED_BRIGHT != LED_BRIGHT_NEW) {
      LED_BRIGHT = LED_BRIGHT_NEW;
      writeCfgFile("led_bright", doc["state1"].as<String>());
      Serial.println("Led brightness was updated in store: " + LED_BRIGHT);
    }
  }

  if (doc["state2"].as<String>()) {
    String MONITOR_SLUG_NEW = doc["state2"].as<String>();
    if (MONITOR_SLUG != MONITOR_SLUG_NEW) {
      MONITOR_SLUG = MONITOR_SLUG_NEW;
      writeCfgFile("monitor_slug", doc["state2"].as<String>());
      Serial.println("Monitor id was updated in store: " + MONITOR_SLUG);
    }
  }

//  if (doc["state3"].as<int>() > 0) {
    int MONITOR_INTERVAL_NEW = doc["state3"].as<int>();
    if (MONITOR_INTERVAL != MONITOR_INTERVAL_NEW) {
      MONITOR_INTERVAL = MONITOR_INTERVAL_NEW;
      writeCfgFile("monitor_interval", doc["state3"].as<String>());
      Serial.println("Monitor interval was updated in store: " + MONITOR_INTERVAL);
    }
//  }

  if (doc["state4"].as<String>()) {
    String MONITOR_NAME_NEW = doc["state4"].as<String>();
    if (MONITOR_NAME != MONITOR_NAME_NEW) {
      MONITOR_NAME = MONITOR_NAME_NEW;
      writeCfgFile("monitor_name", doc["state4"].as<String>());
      Serial.println("Monitor name was updated in store: " + MONITOR_NAME);
    }
  }

//  if (doc["state5"].as<int>()) {
//Serial.println(doc["state5"].as<String>());
//if (doc["state5"].as<String>() != "null") {
//    int SENS_INTERVAL_NEW = doc["state5"].as<int>();
//    if (SENS_INTERVAL != SENS_INTERVAL_NEW) {
//      SENS_INTERVAL = SENS_INTERVAL_NEW;
//      writeCfgFile("sensor_interval", doc["state5"].as<String>());
//      Serial.println("Sensor interval was updated in store: " + SENS_INTERVAL);
//    }
//}
//  }


  return true;
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// // //

void setLedLen(int distance, int targetDistance) {
  if (distance > targetDistance) {
    distance = targetDistance;
  } else if (distance < 25) {
    Serial.println("Too low distance");
    return;
  }
  int ledLen = distance / 25;
  int flc = 0;

  int brightMode = 1; // 15 - distance / 400;
  mtrx.setBright(brightMode);
  mtrx.clear();
  mtrx.update();

  Serial.println(brightMode);
  Serial.println(distance);
  Serial.println(ledLen);

  int lastRow = 0;
  int lastCol = 0;
  bool reverse = false;
  for (int r = 0; r < 32; r++) {
    if (reverse) {
      for (int i = 7; i >= 0; i--) {
        mtrx.dot(r, i);
        lastRow = r;
        lastCol = i;
        flc++;
        if (flc == ledLen) {
          break;
        }
      }
      reverse = false;
    } else {
      for (int i = 0; i < 8; i++) {
        mtrx.dot(r, i);
        lastRow = r;
        lastCol = i;
        flc++;
        if (flc == ledLen) {
          break;
        }
      }
      reverse = true;
    }
    if (flc == ledLen) {
      break;
    }
  }
  mtrx.update();

  flashDot(lastRow, lastCol, reverse);
}

void flashDot(int r, int c, bool reverse) {
  int nextRow = r;
  int nextCol = c;

  for (int i = 0; i < 3; i++) {
    mtrx.dot(nextRow, nextCol, 1);
    mtrx.update();
    delay(500);
    mtrx.dot(nextRow, nextCol, 0);
    mtrx.update();
    delay(500);
  }
}
