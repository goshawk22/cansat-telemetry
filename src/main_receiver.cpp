#include "radio.h"
#include "telemetry.h"
#include "config.h"
#include <Arduino.h>
#include "CommandInterface.h"

CommandInterface cmd;
Radio radio;
Telemetry telemetry;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // Wait for Serial to be ready
    }

    Serial.println("[Main] Starting setup...");

    // Initialize radio
    radio.begin();

    // Start command interface
    cmd.begin(&radio);

    // Stay in command interface until it exits
    while (cmd.isRunning())
    {
        cmd.loop();
        delay(10);
    }

    // Optionally, print a message before continuing
    Serial.println("[Main] Exiting command interface, continuing setup...");

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