#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "config.h"
#include <Arduino.h>

#define BUFFER_SIZE 15 // Size of the telemetry buffer

class Telemetry
{
public:
    Telemetry();

    uint8_t *getBuffer()
    {
        return buffer;
    }

    size_t getSize() const
    {
        return BUFFER_SIZE;
    }

    void fillTelemetryPacket(double latitude, double longitude, float altitude, float speed, uint8_t satellites, float temperature, float humidity, float pressure, float voltage);

private:
    void pack_lat_lon(double lat, double lon);
    uint8_t big_number_ecode(uint8_t A, uint8_t A_min, uint8_t A_max, uint8_t B, uint8_t B_min, uint8_t B_max);
    void big_number_decode(uint8_t C, uint8_t A_min, uint8_t A_max, uint8_t B_min, uint8_t B_max, uint8_t &A_out, uint8_t &B_out);
    uint8_t buffer[BUFFER_SIZE];
};

#endif