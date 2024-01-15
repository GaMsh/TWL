void sendSensorData_BME280(int currentMillis) {
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
    BME280.read(Pressure, Temperature, Humidity, tempUnit, presUnit);
    Pressure = Pressure / 133.3224;
  }

  if (!isnan(Pressure)) {
    String string = String(Temperature) + "::" + String(Humidity) + "::" + String(Pressure);
    callToServer("BME280::" + string);
  }
}

void sendSensorData_HTU21(int currentMillis) {
  float Temperature;
  float Humidity;

  if (CHIP_TEST) {
    Temperature = 25.29;
    Humidity = 12.04;
  } else {
    Temperature = HTU21.readTemperature();
    Humidity = HTU21.getCompensatedHumidity(Temperature);
  }

  if (Temperature != 255 && Temperature != 998 && Temperature != 999) {
    String string = String(Temperature) + "::" + String(Humidity);
    callToServer("HTU21::" + string);
  }
}

void sendSensorData_SHT31(int currentMillis) {
  float Temperature;
  float Humidity;

  if (CHIP_TEST) {
    Temperature = 14.74;
    Humidity = 48.75;
  } else {
    SHT31.UpdateData();
    Temperature = SHT31.GetTemperature();
    Humidity = SHT31.GetRelHumidity();
  }

  if (Temperature != 255 && Temperature != 998 && Temperature != 999 &&
      Humidity != 0) {
    String string = String(Temperature) + "::" + String(Humidity);
    callToServer("SHT31::" + string);
  }
}
