// core functions
bool getDeviceConfiguration(bool first) {
  WiFiClient wifi;
  HTTPClient http;

  http.begin(wifi, String(HTTP_API_SERVER) + "init");
  http.setUserAgent(deviceName);
  http.setTimeout(6000);
  http.addHeader("Content-Type", "application/json");

  JsonDocument jsonRequestData;
  jsonRequestData["mac"] = String(WiFi.macAddress());
  jsonRequestData["deviceId"] = getSensorID();
  jsonRequestData["token"] = TOKEN;
  jsonRequestData["revision"] = String(DEVICE_REVISION);
  jsonRequestData["model"] = String(DEVICE_MODEL);
  jsonRequestData["firmware"] = String(DEVICE_FIRMWARE);
  jsonRequestData["ip"] = WiFi.localIP().toString();
  jsonRequestData["ssid"] = String(WiFi.SSID());
  jsonRequestData["rssi"] = String(ESP.getVcc());
  jsonRequestData["noAutoUpdate"] = NO_AUTO_UPDATE;
  jsonRequestData["chipTest"] = CHIP_TEST;

  String jsonRequest;
  serializeJson(jsonRequestData, jsonRequest);

  int httpCode = http.POST(jsonRequest);
  if (!first && httpCode < 0) {
    NO_SERVER = true;
    return false;
  } else {
    if (httpCode != HTTP_CODE_OK && httpCode != HTTP_CODE_CREATED && !CHIP_TEST) {
      ticker1.attach_ms(200, tickInternal);
      ticker2.attach_ms(500, tickExternal, MAIN_MODE_OFFLINE);
      Serial.println("Не могу инициализировать устройство в БольшоеАпи.ру");
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

  String jsonResponse = http.getString();
  JsonDocument jsonResponseData;
  deserializeJson(jsonResponseData, jsonResponse);
  http.end();

  int serverTime = jsonResponseData["time"].as<int>();
  Serial.println(serverTime);

  struct timeval tv;
  tv.tv_sec = serverTime;
  settimeofday(&tv, NULL); // Применяем время сервера, как локальное, так как у нас нет RTC

  TOKEN = jsonResponseData["token"].as<String>();
  Serial.println("Device token was stored in memory");

  if (jsonResponseData["state1"]) {
      LED_BRIGHT = jsonResponseData["state1"].as<int>();
      Serial.println("Led brightness was stored in memory: " + LED_BRIGHT);
  }

  if (jsonResponseData["state2"]) {
      MONITOR_SLUG = jsonResponseData["state2"].as<String>();
      Serial.println("Monitor id was stored in memory: " + MONITOR_SLUG);
  }

  if (jsonResponseData["state3"]) {
      MONITOR_INTERVAL = jsonResponseData["state3"].as<int>();
      Serial.println("Monitor interval was stored in memory: " + MONITOR_INTERVAL);
  }

  if (jsonResponseData["state4"]) {
      MONITOR_NAME = jsonResponseData["state4"].as<String>();
      Serial.println("Monitor name was stored in memory: " + MONITOR_NAME);
  }

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

float decimalRound(float input, int decimals)
{
  float scale=pow(10, decimals);
  return round(input * scale) / scale;
}

//////

//void setLedLen(int distance, int targetDistance) {
//  if (distance > targetDistance) {
//    distance = targetDistance;
//  } else if (distance < 25) {
//    Serial.println("Too low distance");
//    return;
//  }
//  int ledLen = distance / 25;
//  int flc = 0;
//
//  int brightMode = 1; // 15 - distance / 400;
//  mtrx.setBright(brightMode);
//  mtrx.clear();
//  mtrx.update();
//
//  Serial.println(brightMode);
//  Serial.println(distance);
//  Serial.println(ledLen);
//
//  int lastRow = 0;
//  int lastCol = 0;
//  bool reverse = false;
//  for (int r = 0; r < 32; r++) {
//    if (reverse) {
//      for (int i = 7; i >= 0; i--) {
//        mtrx.dot(r, i);
//        lastRow = r;
//        lastCol = i;
//        flc++;
//        if (flc == ledLen) {
//          break;
//        }
//      }
//      reverse = false;
//    } else {
//      for (int i = 0; i < 8; i++) {
//        mtrx.dot(r, i);
//        lastRow = r;
//        lastCol = i;
//        flc++;
//        if (flc == ledLen) {
//          break;
//        }
//      }
//      reverse = true;
//    }
//    if (flc == ledLen) {
//      break;
//    }
//  }
//  mtrx.update();
//
//  flashDot(lastRow, lastCol, reverse);
//}
//
//void flashDot(int r, int c, bool reverse) {
//  int nextRow = r;
//  int nextCol = c;
//
//  for (int i = 0; i < 3; i++) {
//    mtrx.dot(nextRow, nextCol, 1);
//    mtrx.update();
//    delay(500);
//    mtrx.dot(nextRow, nextCol, 0);
//    mtrx.update();
//    delay(500);
//  }
//}
