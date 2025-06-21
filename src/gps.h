#ifndef GPS_H
#define GPS_H

#include "config.h"
#include <TinyGPS++.h>
#include <SerialPIO.h>

#define GPS_FIX_AGE 5000 // Maximum age of GPS fix in milliseconds

class GPS
{
public:
    GPS(SerialPIO &serialPort, uint32_t baudRate = GPS_BAUD_RATE);
    void begin();
    void update();

    // Accessors
    bool isValidFix() const
    {
        return gps.location.isValid() && gps.location.age() < GPS_FIX_AGE;
    }
    double getLatitude() const
    {
        return latitude;
    }
    double getLongitude() const
    {
        return longitude;
    }
    double getSpeed() const
    {
        return speed;
    }
    double getAltitude() const
    {
        return altitude;
    }
    int getSatellites() const
    {
        return satellites;
    }
    uint32_t getTime() const
    {
        return time;
    }

private:
    TinyGPSPlus gps;
    SerialPIO &gpsSerial;
    uint32_t gpsBaud;

    // Cached data
    double latitude;
    double longitude;
    double altitude;
    int satellites;
    int speed;
    uint32_t time;
};

#endif