JsonDocument getSensorData_BME280(int currentMillis) {
  Serial.println("Sensor read: BME280");
  float Temperature;
  float Humidity;
  float Pressure;

  if (CHIP_TEST) {
    Temperature = 25.2;
    Humidity = 65.9;
    Pressure = 762.6;
  } else {
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);
    BME280.read(Pressure, Temperature, Humidity, tempUnit, presUnit);
    Pressure = Pressure / 133.3224;
  }

  JsonDocument doc;
  if (!isnan(Pressure)) {
    doc["temperature"] = (String)DecimalRound(Temperature, 1);
    doc["humidity"] = (String)DecimalRound(Humidity, 1);
    doc["pressure"] = (String)DecimalRound(Pressure, 1);
  }
  return doc;
}

JsonDocument getSensorData_HTU21(int currentMillis) {
  Serial.println("Sensor read: HTU21");
  float Temperature;
  float Humidity;

  if (CHIP_TEST) {
    Temperature = 25.2;
    Humidity = 12.5;
  } else {
    Temperature = HTU21.readTemperature();
    Humidity = HTU21.getCompensatedHumidity(Temperature);
  }

    JsonDocument doc;
  if (Temperature != 255 && Temperature != 998 && Temperature != 999) {
    doc["temperature"] = (String)DecimalRound(Temperature, 1);
    doc["humidity"] = (String)DecimalRound(Humidity, 1);
  }
  return doc;
}

JsonDocument getSensorData_SHT31(int currentMillis) {
  Serial.println("Sensor read: SHT31");
  float Temperature;
  float Humidity;

  if (CHIP_TEST) {
    Temperature = 14.7;
    Humidity = 48.7;
  } else {
    SHT31.UpdateData();
    Temperature = SHT31.GetTemperature();
    Humidity = SHT31.GetRelHumidity();
  }

  JsonDocument doc;
  if (Temperature != 255 && Temperature != 998 && Temperature != 999 && Humidity != 0) {
    doc["temperature"] = (String)DecimalRound(Temperature, 1);
    doc["humidity"] = (String)DecimalRound(Humidity, 1);
  }
  return doc;
}
