#include "sensors.h"

Sensors::Sensors()
{
}

void Sensors::begin()
{
    analogReadResolution(12); // Set ADC resolution to 12 bits
    Wire.setSDA(SENSOR_SDA_PIN);
    Wire.setSCL(SENSOR_SCL_PIN);
    Wire.begin();
    if (!aht.begin(&Wire))
    {
        DEBUG_PRINTLN("[AHT20] Failed to find AHT20 sensor!");
    }
    aht_temp = aht.getTemperatureSensor();
    aht_humidity = aht.getHumiditySensor();

    if (!bme.begin(0x77, &Wire))
    {
        DEBUG_PRINTLN("[BME280] Failed to find BME280 sensor!");
    }

    bme_temp = bme.getTemperatureSensor();
    bme_pressure = bme.getPressureSensor();
    bme_humidity = bme.getHumiditySensor();
}

void Sensors::update()
{
    DEBUG_PRINTLN("[Sensors] Updating sensor readings...");
    sensors_event_t aht_humidity_event, aht_temp_event, bme_temp_event, bme_pressure_event, bme_humidity_event;
    aht_humidity->getEvent(&aht_humidity_event);
    aht_temp->getEvent(&aht_temp_event);
    bme_temp->getEvent(&bme_temp_event);
    bme_pressure->getEvent(&bme_pressure_event);
    bme_humidity->getEvent(&bme_humidity_event);

    temperature = aht_temp_event.temperature;
    relative_humidity = aht_humidity_event.relative_humidity;
    pressure = bme_pressure_event.pressure;
    float v_adc = analogRead(VSYS_ADC_PIN) * 3.3 / 4095.0; // 12-bit ADC
    voltage = v_adc * VOLTAGE_DIVIDER_MULTIPLIER;
}