#include "telemetry.h"

Telemetry::Telemetry()
{
    // Initialize the buffer to zero
    memset(buffer, 0, BUFFER_SIZE);
}

// Fill the telemetry packet with the provided data
void Telemetry::fillTelemetryPacket(double latitude, double longitude, float altitude, float speed, uint8_t satellites, float temperature, float humidity, float pressure, float voltage)
{
    pack_lat_lon(latitude, longitude);

    // Pack altitude
    uint16_t altitudeBinary = (uint16_t)((altitude - ALTITUDE_MIN) / (ALTITUDE_MAX - ALTITUDE_MIN) * 65535);
    buffer[6] = (altitudeBinary >> 8) & 0xFF;
    buffer[7] = altitudeBinary & 0xFF;

    // Pack speed
    buffer[8] = (uint8_t)((speed - SPEED_MIN) / (SPEED_MAX - SPEED_MIN) * 255);

    // Pack temperature
    uint16_t tempBinary = (uint16_t)((temperature - AHT20_TEMPERATURE_MIN) / (AHT20_TEMPERATURE_MAX - AHT20_TEMPERATURE_MIN) * 65535);
    buffer[9] = (tempBinary >> 8) & 0xFF;
    buffer[10] = tempBinary & 0xFF;

    // Pack humidity
    buffer[11] = (uint8_t)((humidity - AHT20_HUMIDITY_MIN) / (AHT20_HUMIDITY_MAX - AHT20_HUMIDITY_MIN) * 255);

    // Pack pressure
    uint16_t pressureBinary = (uint16_t)((pressure - BME280_PRESSURE_MIN) / (BME280_PRESSURE_MAX - BME280_PRESSURE_MIN) * 65535);
    buffer[12] = (pressureBinary >> 8) & 0xFF;
    buffer[13] = pressureBinary & 0xFF;

    // Use big_number_ecode to pack satellites and voltage
    buffer[14] = big_number_ecode((uint8_t)(voltage * 10), VOLTAGE_MIN * 10, VOLTAGE_MAX * 10, satellites, SATELLITES_MIN, SATELLITES_MAX);
}

/**
 * Store Lat & Long in six bytes of payload
 * Give about a few meters of accuracy
 */
void Telemetry::pack_lat_lon(double lat, double lon)
{
    uint32_t LatitudeBinary = ((lat + 90) / 180.0) * 16777215;
    uint32_t LongitudeBinary = ((lon + 180) / 360.0) * 16777215;

    buffer[0] = (LatitudeBinary >> 16) & 0xFF;
    buffer[1] = (LatitudeBinary >> 8) & 0xFF;
    buffer[2] = LatitudeBinary & 0xFF;
    buffer[3] = (LongitudeBinary >> 16) & 0xFF;
    buffer[4] = (LongitudeBinary >> 8) & 0xFF;
    buffer[5] = LongitudeBinary & 0xFF;
}

/**
 * Store two variables in one byte (make sure they fit)
 * A and B are encoded in a single byte C
 * A is in range [0, A_range]
 * B is in range [0, B_range]
 * C is in range [0, A_range + B_range]
 * This is useful for packing two small numbers into one byte, saving on bandwidth
 */
uint8_t Telemetry::big_number_ecode(uint8_t A, uint8_t A_min, uint8_t A_max, uint8_t B, uint8_t B_min, uint8_t B_max)
{
    uint8_t A_range = A_max - A_min + 1;
    uint8_t B_range = B_max - B_min + 1;

    uint8_t C = 0;
    C = C * A_range + (A - A_min);
    C = C * B_range + (B - B_min);
    return C;
}