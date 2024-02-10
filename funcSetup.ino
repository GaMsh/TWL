void resetWiFiSettings() {
  ticker1.attach_ms(512, tickInternal);
  Serial.println("WiFi reset by special PIN");
  WiFi.disconnect(true);
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.eraseConfig();
  delay(2000);
  ESP.reset();
}

void checkWiFiConfiguration() {
  if (WiFi.SSID() != "") {
    Serial.print("Current Saved WiFi SSID: ");
    Serial.println(WiFi.SSID());

    if (NO_AUTO_UPDATE == 0) {
      // reset wifi by touch RESET_WIFI pin to GROUND
      int resetCycle = 0;
      ticker1.attach_ms(36, tickInternal);
      while (resetCycle < 42) {
        MODE_RESET_WIFI = digitalRead(RESET_WIFI);
        if (MODE_RESET_WIFI == LOW) {
          resetWiFiSettings();
          break;
        }
        resetCycle++;
        delay(36);
      }
    }
  } else {
    Serial.println("We don`t have saved WiFi settings, need configure");
  }
}

void manualCheckFirmwareUpdate() {
  // reset firmware by touch RESET_WIFI pin to GROUND
  analogWrite(LED_EXTERNAL, 0);
  int resetCycle = 0;
  ticker1.attach_ms(200, tickInternal);
  while (resetCycle < 125) {
    MODE_RESET_WIFI = digitalRead(RESET_WIFI);
    if (MODE_RESET_WIFI == LOW) {
      Serial.println("MANUAL UPDATE");
      checkFirmwareUpdate(true);
      break;
    }
    resetCycle++;
    delay(36);
  }
  analogWrite(LED_EXTERNAL, 255);
}

bool setupWiFiManager() {
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setMinimumSignalQuality(33);
  wifiManager.setRemoveDuplicateAPs(true);
  wifiManager.setDebugOutput(false);
  wifiManager.setCustomHeadElement("<style>html{background:#faa55a};</style>");

  if (wifiManager.autoConnect()) {
    return true;
  }

  if (WiFi.SSID() == "") {
    String wifiPortalSsid = deviceName + "_SETUP_" + getSensorID();
    wifiManager.startConfigPortal(wifiPortalSsid.c_str());
  }

  return false;
}

String readCfgFile(String configVar) {
  String result = "";

  File file = LittleFS.open("/" + configVar + ".cfg", "r");
  if (file) {
    result = file.readString();
    Serial.print(configVar + ": ");
    Serial.println(result);
    file.close();
  } else {
      Serial.println("Problem read " + configVar + ".cfg");
  }

  return result;
}

int writeCfgFile(String configVar, String value) {
  File file = LittleFS.open("/" + configVar + ".cfg", "w");
  if (file) {
    Serial.println("Write file " + configVar + ".cfg");
    bytesWriten = file.print(value);
    if (bytesWriten > 0) {
      Serial.print("File was written: ");
      Serial.println(bytesWriten);
      return bytesWriten;
    } else {
      Serial.println("File write failed");
      return 0;
    }
    file.close();
  }

  return -1;
}

String getSensorID() { return String(ESP.getChipId()); }
