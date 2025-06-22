#include "radio.h"
#include "sensors.h"
#include "gps.h"
#include "telemetry.h"
#include "config.h"
#include <Arduino.h>

Radio radio;
Sensors sensors;
SerialPIO gpsSerial(GPS_TX_PIN, GPS_RX_PIN);
GPS gps(gpsSerial, GPS_BAUD_RATE);
Telemetry telemetry;

void setup()
{
    Serial.begin(115200);
#ifdef DEBUG
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
#endif

    Serial.println("[Main] Starting setup...");

    // Initialize radio
    radio.begin();

    // Initialize sensors
    sensors.begin();

    // Initialize GPS
    gps.begin();

    Serial.println("[Main] Setup completed.");
}

void loop()
{
    static unsigned long lastPrintTime = 0;
    static unsigned long lastTransmitTime = 0;

    // Update GPS
    gps.update();

    // Update radio
    radio.update();

    // Respect 10% duty cycle: transmit at most once every 10 seconds
    if (radio.isReady() && (millis() - lastTransmitTime > LORA_INTERVAL))
    {
        lastTransmitTime = millis();

        // Update sensors
        sensors.update();

        // Prepare and transmit telemetry packet
        telemetry.fillTelemetryPacket(gps.getLatitude(), gps.getLongitude(), gps.getAltitude(),
                                      gps.getSpeed(), gps.getSatellites(), sensors.getTemperature(),
                                      sensors.getRelativeHumidity(), sensors.getPressure(), sensors.getVoltage());

        radio.sendPacket(telemetry.getBuffer(), telemetry.getSize());
    }

    if (millis() - lastPrintTime > 5000)
    {
        lastPrintTime = millis();

        // Print sensor data
        VERBOSE_PRINT("Temperature: ");
        VERBOSE_PRINT(sensors.getTemperature());
        VERBOSE_PRINTLN(" °C");

        VERBOSE_PRINT("Humidity: ");
        VERBOSE_PRINT(sensors.getRelativeHumidity());
        VERBOSE_PRINTLN(" %");

        VERBOSE_PRINT("Pressure: ");
        VERBOSE_PRINT(sensors.getPressure());
        VERBOSE_PRINTLN(" hPa");

        VERBOSE_PRINT("Voltage: ");
        VERBOSE_PRINT(sensors.getVoltage());
        VERBOSE_PRINTLN(" V");

        if (gps.isValidFix())
        {
            VERBOSE_PRINT("GPS Latitude: ");
            VERBOSE_PRINTLN(gps.getLatitude());
            VERBOSE_PRINT("GPS Longitude: ");
            VERBOSE_PRINTLN(gps.getLongitude());
            VERBOSE_PRINT("GPS Altitude: ");
            VERBOSE_PRINTLN(gps.getAltitude());
        }
        else
        {
            VERBOSE_PRINTLN("GPS fix not valid.");
        }
    }
}
