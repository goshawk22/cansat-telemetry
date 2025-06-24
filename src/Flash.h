#ifndef FLASH_H
#define FLASH_H

#include "config.h"
#include <Arduino.h>
#include <LittleFS.h>

#define FILENAME_PREFIX "flight_"
#define FILENAME_SUFFIX ".csv"

class Flash
{
public:
    Flash();
    void begin();
    void write(uint32_t time, double latitude, double longitude, float altitude, float speed, uint8_t satellites, float temperature, float humidity, float pressure, float voltage);
    void createFile();
    std::vector<String> listFlights();
    void formatFlightData();
    void read(String flight_filename);
    bool isInitialized() const
    {
        return initialized;
    }

private:
    void headerCSV();
    String filename;
    bool initialized = false;
};

#endif