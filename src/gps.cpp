// Some of the following code is based on the following resource by Stuard Robinson:
// https://stuartsprojects.github.io/2024/09/21/How-not-to-read-a-GPS.html

#include "gps.h"

// checksums for nmea sentences are calculated using https://www.nmeachecksum.eqth.net
#define NMEA_CONFIG_STRING "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define NMEA_INTERVAL_STRING "$PMTK220,500*2B"

GPS::GPS(SerialPIO &serialPort, uint32_t baudRate)
    : gpsSerial(serialPort), gpsBaud(baudRate),
      latitude(0.0), longitude(0.0), altitude(0.0), satellites(0), speed(0.0), time(0)
{
}

void GPS::begin()
{
    // Initialize the serial port for GPS communication
    gpsSerial.begin(gpsBaud);
    // Want to set up the GPS to only output certain sentences
    gpsSerial.println(NMEA_CONFIG_STRING);
    gpsSerial.println(NMEA_INTERVAL_STRING);
    DEBUG_PRINTLN("[GPS] Initialized.");
}

void GPS::update()
{
    // Read data from the GPS serial port
    while (gpsSerial.available())
    {
        char c = gpsSerial.read();
        // Serial.print(c);
        gps.encode(c);
    }
    // Update cached data if a valid fix is available
    if (gps.location.isValid() &&  gps.location.age() < 2000)
    {
        // DEBUG_PRINTLN("[GPS] Valid fix received.");
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        altitude = gps.altitude.meters();
        satellites = gps.satellites.value();
        speed = gps.speed.kmph();
        time = gps.time.value();
    } else
    {
        // DEBUG_PRINTLN("[GPS] No valid fix.");
        latitude = 0.0;
        longitude = 0.0;
        altitude = 0.0;
        satellites = gps.satellites.value(); // satellites will still be valid even if no fix
        speed = 0.0;
        time = gps.time.value(); // even if no fix, the time will still be updated
    }
}