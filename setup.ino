void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_EXTERNAL, OUTPUT);
    pinMode(RESET_WIFI, INPUT_PULLUP);

    pinMode(D0, OUTPUT);

    Serial.begin(SERIAL_BAUD);
    while (!Serial) {}

    Serial.println("Device '" + deviceName + "' is starting...");
    Serial.println("Voltage: " + String(ESP.getVcc()));

    checkWiFiConfiguration();

    if (CHIP_TEST) {
        Serial.println("CHIP TEST mode is activated. No real data from sensors in this mode");
    }
    if (NO_AUTO_UPDATE) {
        Serial.println("NO AUTO UPDATE firmware mode is activated! You can manually update, by RESET_WIFI pin to LOW on boot");
        manualCheckFirmwareUpdate();
    }

    WiFi.hostname(deviceName);

    ticker1.attach_ms(100, tickInternal);
    ticker2.attach_ms(100, tickExternal, MAIN_MODE_OFFLINE);

    if (!setupWiFiManager()) {
        delay(15000);
        ESP.restart();
    } else {
        Serial.println("WiFi network connected (" + String(WiFi.RSSI()) + ")");
        NO_INTERNET = false;

        checkFirmwareUpdate(false);

        if (LittleFS.begin()) {
            Serial.println(F("LittleFS was mounted"));
        } else {
            Serial.println(F("Error while mounting LittleFS"));
        }

        int customLedBright = readCfgFile("led_bright").toInt();
        if (customLedBright > 0) {
            LED_BRIGHT = customLedBright;
        }

        ///// Final
        TOKEN = readCfgFile("token");

        ticker2.attach_ms(500, tickExternal, MAIN_MODE_OFFLINE);
        getDeviceConfiguration(true);
        tickOffAll();
        ticker1.attach_ms(100, tickInternal);

        if (bufferCount("data") > 0) {
            Serial.println();
            Serial.println("Buffer count: " + bufferCount("data"));
            MODE_SEND_BUFFER = true;
        }

        if (!CHIP_TEST) {
            tickOffAll();
            ticker1.attach_ms(250, tickInternal);
            ticker2.attach_ms(1000, tickExternal, MAIN_MODE_NORMAL);
            
            Wire.begin();
            INDOOR_SENSOR1.begin();
            INDOOR_SENSOR2.Begin();
            
            tickOffAll();
            ticker1.attach_ms(250, tickInternal);
            ticker2.attach_ms(2000, tickExternal, MAIN_MODE_NORMAL);
            int tryBMERemaining = 11;
            while (!OUTDOOR_SENSOR.begin()) {
                if (tryBMERemaining == 0) {
                    STATUS_OUTDOOR_GOOD = false;
                    break;
                }

                delay(290);
                tryBMERemaining--;
            }
        }
    }

    tickOffAll();

    // ?????????????????? ?????????????????????????? ????????????????????, ???????????????????? ?????????????? ???????????????????? ???? ??????????????
    Serial.println("Init complete");

    analogWrite(LED_EXTERNAL, 255);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    analogWrite(LED_EXTERNAL, LED_BRIGHT);

    // // //

//    digitalWrite(D0, HIGH);
//    mtrx.begin();        // ??????????????????
//    mtrx.setBright(0);  // ?????????????? 0..15
//    mtrx.setRotation(0); // ?????????? ?????????????????? 0..3, ???? 90 ???????? ???? ?????????????? ??????????????
//    setLedLen(5000);
//    delay(1000);
//    mtrx.clear();
//    mtrx.update();
//    digitalWrite(D0, LOW);
}
