#include "radio.h"
#include "sensors.h"
#include "gps.h"
#include "telemetry.h"
#include "config.h"
#include <Arduino.h>
#include "CommandInterface.h"
#include "Flash.h"

Radio radio;
Sensors sensors;
SerialPIO gpsSerial(GPS_TX_PIN, GPS_RX_PIN);
GPS gps(gpsSerial, GPS_BAUD_RATE);
Telemetry telemetry;
CommandInterface cmd;
Flash flash;

void setup()
{
    Serial.begin(115200);
    unsigned long serialWaitStart = millis();
    bool serialConnected = false;
    while (!Serial && (millis() - serialWaitStart < 5000))
    {
        ; // wait for serial port to connect, but only up to 5 seconds
    }
    if (Serial)
    {
        serialConnected = true;
        cmd.begin(&flash); // Start command interface

        // Stay in command interface until it exits
        while (cmd.isRunning())
        {
            cmd.loop();
            delay(10);
        }

        // Optionally, print a message before continuing
        Serial.println("[Main] Exiting command interface, continuing setup...");
    }

    Serial.println("[Main] Starting setup...");

    // Initialize radio
    radio.begin();

    // Intitialize flash memory
    flash.begin();

    // Create a new file for flight data
    flash.createFile();

    // Initialize sensors
    sensors.begin();

    // Initialize GPS
    gps.begin();

    Serial.println("[Main] Setup completed.");
    DEBUG_PRINTLN("[Main] System is waiting for START command.");
    // Wait for the START command from the receiver
    bool gotStartCommand = false;
    while (!gotStartCommand)
    {
        // Check if the radio is ready to receive
        uint8_t buffer[5];
        int state = radio.receivePacket(buffer, sizeof(buffer));
        if (state == RADIOLIB_ERR_NONE)
        {
            if (strncmp((const char *)buffer, "START", 5) == 0)
            {
                Serial.println("[Main] Received START command.");
                gotStartCommand = true; // We received the START command
                // Send acknowledgment back to the receiver
                DEBUG_PRINTLN("[Main] Sending ACK to receiver.");
                radio.sendPacket((const uint8_t *)"ACK", 3);
                while (!radio.isReady())
                {
                    // Wait until the radio has sent the ACK
                    radio.update();
                }
            }
            else
            {
                DEBUG_PRINTLN("[Main] Received unknown packet.");
            }
        }
        else
        {
            DEBUG_PRINT("[Main] Failed to receive packet, error code: ");
            DEBUG_PRINTLN(state);
        }
    }
}

void loop()
{
    static unsigned long lastPrintTime = 0;
    static unsigned long lastWriteTime = 0;
    static unsigned long lastTransmitTime = 0;

    // Update GPS
    gps.update();

    // Update radio
    radio.update();

    // Respect 10% duty cycle: transmit at most once every LORA_INTERVAL milliseconds
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

    // Write telemetry data to flash memory and time how long it takes
    if (flash.isInitialized() && (millis() - lastWriteTime > FLASH_INTERVAL))
    {
        lastWriteTime = millis();

        // Update sensors
        sensors.update();

        // Write telemetry data to flash memory
        flash.write(gps.getTime(), gps.getLatitude(), gps.getLongitude(),
                    gps.getAltitude(), gps.getSpeed(), gps.getSatellites(),
                    sensors.getTemperature(), sensors.getRelativeHumidity(),
                    sensors.getPressure(), sensors.getVoltage());
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
