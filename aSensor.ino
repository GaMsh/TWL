void sendSensorData_OUTDOOR(int currentMillis) {
    float Temperature;
    float Humidity;
    float Pressure;

    if (CHIP_TEST) {
        Temperature = 25.29;
        Humidity = 65.93;
        Pressure = 762.6;
    } else {
        BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
        BME280::PresUnit presUnit(BME280::PresUnit_Pa);

        OUTDOOR_SENSOR.read(Pressure, Temperature, Humidity, tempUnit, presUnit);
        Pressure = Pressure / 133.3224;
    }

    if (!isnan(Pressure)) {
      String string = String(Temperature) + "::" + String(Humidity) + "::" + String(Pressure);
      callToServer("OUTDOOR::" + string);
    }
}

void sendSensorData_INDOOR1(int currentMillis) {
    float Temperature;
    float Humidity;

    if (CHIP_TEST) {
        Temperature = 25.29;
        Humidity = 12.04;
    } else {
        Temperature = INDOOR_SENSOR1.readTemperature();
        Humidity = INDOOR_SENSOR1.getCompensatedHumidity(Temperature);
    }

    if (Temperature != 255 && Temperature != 998 && Temperature != 999) {
      String string = String(Temperature) + "::" + String(Humidity);
      callToServer("INDOOR1::" + string);
    }
}


void sendSensorData_INDOOR2(int currentMillis) {
    float Temperature;
    float Humidity;

    if (CHIP_TEST) {
        Temperature = 14.74;
        Humidity = 48.75;
    } else {
        INDOOR_SENSOR2.UpdateData();
        Temperature = INDOOR_SENSOR2.GetTemperature();
        Humidity = INDOOR_SENSOR2.GetRelHumidity();
    }

    if (Temperature != 255 && Temperature != 998 && Temperature!= 999 && Humidity != 0) {
      String string = String(Temperature) + "::" + String(Humidity);
      callToServer("INDOOR2::" + string);
    }
}
