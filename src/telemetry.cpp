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
    if (altitude < ALTITUDE_MIN)
    {
        altitude = ALTITUDE_MIN;
    }
    else if (altitude > ALTITUDE_MAX)
    {
        altitude = ALTITUDE_MAX;
    }
    uint16_t altitudeBinary = (uint16_t)((altitude - ALTITUDE_MIN) / (ALTITUDE_MAX - ALTITUDE_MIN) * 65535);
    buffer[6] = (altitudeBinary >> 8) & 0xFF;
    buffer[7] = altitudeBinary & 0xFF;

    // Pack speed
    if (speed < SPEED_MIN)
    {
        speed = SPEED_MIN;
    }
    else if (speed > SPEED_MAX)
    {
        speed = SPEED_MAX;
    }
    buffer[8] = (uint8_t)((speed - SPEED_MIN) / (SPEED_MAX - SPEED_MIN) * 255);

    // Pack temperature
    if (temperature < AHT20_TEMPERATURE_MIN)
    {
        temperature = AHT20_TEMPERATURE_MIN;
    }
    else if (temperature > AHT20_TEMPERATURE_MAX)
    {
        temperature = AHT20_TEMPERATURE_MAX;
    }
    
    uint16_t tempBinary = (uint16_t)((temperature - AHT20_TEMPERATURE_MIN) / (AHT20_TEMPERATURE_MAX - AHT20_TEMPERATURE_MIN) * 65535);
    buffer[9] = (tempBinary >> 8) & 0xFF;
    buffer[10] = tempBinary & 0xFF;

    // Pack humidity
    if (humidity < AHT20_HUMIDITY_MIN)
    {
        humidity = AHT20_HUMIDITY_MIN;
    }
    else if (humidity > AHT20_HUMIDITY_MAX)
    {
        humidity = AHT20_HUMIDITY_MAX;
    }
    buffer[11] = (uint8_t)((humidity - AHT20_HUMIDITY_MIN) / (AHT20_HUMIDITY_MAX - AHT20_HUMIDITY_MIN) * 255);

    // Pack pressure
    if (pressure < BME280_PRESSURE_MIN)
    {
        pressure = BME280_PRESSURE_MIN;
    }
    else if (pressure > BME280_PRESSURE_MAX)
    {
        pressure = BME280_PRESSURE_MAX;
    }
    uint16_t pressureBinary = (uint16_t)((pressure - BME280_PRESSURE_MIN) / (BME280_PRESSURE_MAX - BME280_PRESSURE_MIN) * 65535);
    buffer[12] = (pressureBinary >> 8) & 0xFF;
    buffer[13] = pressureBinary & 0xFF;

    // Use big_number_ecode to pack satellites and voltage
    if (satellites < SATELLITES_MIN)
    {
        satellites = SATELLITES_MIN;
    }
    else if (satellites > SATELLITES_MAX)
    {
        satellites = SATELLITES_MAX;
    }
    if (voltage < VOLTAGE_MIN)
    {
        voltage = VOLTAGE_MIN;
    }
    else if (voltage > VOLTAGE_MAX)
    {
        voltage = VOLTAGE_MAX;
    }
    buffer[14] = big_number_encode((uint8_t)(voltage * 10), VOLTAGE_MIN * 10, VOLTAGE_MAX * 10, satellites, SATELLITES_MIN, SATELLITES_MAX);
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
uint8_t Telemetry::big_number_encode(uint8_t A, uint8_t A_min, uint8_t A_max, uint8_t B, uint8_t B_min, uint8_t B_max)
{
    uint8_t A_range = A_max - A_min + 1;
    uint8_t B_range = B_max - B_min + 1;

    uint8_t C = 0;
    C = C * A_range + (A - A_min);
    C = C * B_range + (B - B_min);
    return C;
}

// Decode the big_number_ecode packed byte
void Telemetry::big_number_decode(uint8_t packed, uint8_t A_min, uint8_t A_max, uint8_t B_min, uint8_t B_max, uint8_t &A, uint8_t &B) {
    uint8_t A_range = A_max - A_min + 1;
    uint8_t B_range = B_max - B_min + 1;

    // Reverse the packing process
    B = (packed % B_range) + B_min;
    packed /= B_range;
    A = (packed % A_range) + A_min;
}

// Decode the telemetry packet
void Telemetry::decodeTelemetryPacket(double &latitude, double &longitude,
                           float &altitude, float &speed, uint8_t &satellites,
                           float &temperature, float &humidity, float &pressure, float &voltage)
{
    // Unpack latitude
    uint32_t lat_enc = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
    latitude = ((double)lat_enc) * 180.0 / 16777215.0 - 90.0;

    // Unpack longitude
    uint32_t lon_enc = (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];
    longitude = ((double)lon_enc) * 360.0 / 16777215.0 - 180.0;

    // Unpack altitude
    uint16_t alt_enc = (buffer[6] << 8) | buffer[7];
    altitude = ((float)alt_enc) * (ALTITUDE_MAX - ALTITUDE_MIN) / 65535.0 + ALTITUDE_MIN;

    // Unpack speed
    speed = ((float)buffer[8]) * (SPEED_MAX - SPEED_MIN) / 255.0 + SPEED_MIN;

    // Unpack temperature
    uint16_t temp_enc = (buffer[9] << 8) | buffer[10];
    temperature = ((float)temp_enc) * (AHT20_TEMPERATURE_MAX - AHT20_TEMPERATURE_MIN) / 65535.0 + AHT20_TEMPERATURE_MIN;

    // Unpack humidity
    humidity = ((float)buffer[11]) * (AHT20_HUMIDITY_MAX - AHT20_HUMIDITY_MIN) / 255.0 + AHT20_HUMIDITY_MIN;

    // Unpack pressure
    uint16_t press_enc = (buffer[12] << 8) | buffer[13];
    pressure = ((float)press_enc) * (BME280_PRESSURE_MAX - BME280_PRESSURE_MIN) / 65535.0 + BME280_PRESSURE_MIN;

    // Unpack voltage and satellites
    uint8_t voltage_enc, sats_enc;
    big_number_decode(buffer[14], VOLTAGE_MIN * 10, VOLTAGE_MAX * 10, SATELLITES_MIN, SATELLITES_MAX, voltage_enc, sats_enc);
    voltage = voltage_enc / 10.0;
    satellites = sats_enc;
}