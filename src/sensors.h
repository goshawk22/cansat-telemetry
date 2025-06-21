#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BME280.h>

class Sensors
{
public:
    Sensors();
    void begin();
    void update();

    // Accessors
    float getTemperature() const
    {
        return temperature;
    }
    float getRelativeHumidity() const
    {
        return relative_humidity;
    }
    float getPressure() const
    {
        return pressure;
    }
    float getVoltage() const
    {
        return voltage;
    }

private:
    Adafruit_AHTX0 aht;
    Adafruit_Sensor *aht_humidity, *aht_temp, *bme_temp, *bme_pressure, *bme_humidity;
    Adafruit_BME280 bme;
    // Cached data
    float temperature;
    float relative_humidity;
    float pressure;
    float voltage;
};

#endif