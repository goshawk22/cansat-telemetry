#include "radio.h"
#include "telemetry.h"
#include "config.h"
#include <Arduino.h>

Radio radio;
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

    Serial.println("[Main] Setup completed.");
}


void loop()
{
    double latitude, longitude;
    float altitude, speed, temperature, humidity, pressure, voltage, rssi, snr, frequencyError;
    uint8_t satellites;
    radio.receivePacket(telemetry.getBuffer(), telemetry.getSize());
    telemetry.decodeTelemetryPacket(latitude, longitude, altitude, speed, satellites, temperature, humidity, pressure, voltage);
    rssi = radio.getRSSI();
    snr = radio.getSNR();
    frequencyError = radio.getFrequencyError();
    Serial.print(latitude, 6);
    Serial.print(",");
    Serial.print(longitude, 6);
    Serial.print(",");
    Serial.print(altitude, 2);
    Serial.print(",");
    Serial.print(speed, 2);
    Serial.print(",");
    Serial.print(satellites);
    Serial.print(",");
    Serial.print(temperature, 2);
    Serial.print(",");
    Serial.print(humidity, 2);
    Serial.print(",");
    Serial.print(pressure, 2);
    Serial.print(",");
    Serial.print(voltage, 2);
    Serial.print(",");
    Serial.print(rssi, 2);
    Serial.print(",");
    Serial.print(snr, 2);
    Serial.print(",");
    Serial.println(frequencyError, 2);
}