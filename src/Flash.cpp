#include "Flash.h"

Flash::Flash() {}

void Flash::begin()
{
    // Check if already initialized
    if (initialized)
    {
        DEBUG_PRINTLN("[Flash] Already initialized.");
        return; // Exit if already initialized
    }

    initialized = true; // Set initialized to true

    // Initialize flash memory or any other required setup
    DEBUG_PRINTLN("[Flash] Initialization started.");

    // Begin LittleFS
    if (LittleFS.begin())
    {
        DEBUG_PRINTLN("[Flash] LittleFS initialized successfully.");
    }
    else
    {
        DEBUG_PRINTLN("[Flash] Failed to initialize LittleFS.");
        return; // Exit if initialization fails
    }

    DEBUG_PRINTLN("[Flash] Initialization completed.");
}

void Flash::createFile()
{
    // Find the next available filename
    int fileIndex = 1;
    while (true)
    {
        filename = FILENAME_PREFIX + String(fileIndex) + FILENAME_SUFFIX;
        if (!LittleFS.exists(filename))
        {
            break; // Found an available filename
        }
        fileIndex++;
    }
    DEBUG_PRINT("[Flash] Using filename: ");
    DEBUG_PRINTLN(filename.c_str());

    headerCSV(); // Create the CSV header
    DEBUG_PRINTLN("[Flash] Created File: " + filename);
}

void Flash::write(uint32_t time, double latitude, double longitude, float altitude, float speed, uint8_t satellites, float temperature, float humidity, float pressure, float voltage)
{
    File f = LittleFS.open(filename, "a");
    if (!f)
    {
        DEBUG_PRINTLN("[Flash] Failed to open file for writing.");
        return; // Exit if file cannot be opened
    }

    DEBUG_PRINT("[Flash] Writing: ");

    // Write to the file
    String line = String(time) + "," +
                  String(latitude, 6) + "," +
                  String(longitude, 6) + "," +
                  String(altitude, 2) + "," +
                  String(speed, 2) + "," +
                  String(satellites) + "," +
                  String(temperature, 2) + "," +
                  String(humidity, 2) + "," +
                  String(pressure, 2) + "," +
                  String(voltage, 2) + "\n";

    DEBUG_PRINTLN(line.c_str());
    f.write(line.c_str());

    f.close();
}

void Flash::headerCSV()
{
    File f = LittleFS.open(filename, "w");
    f.printf("time,lat,long,alt,speed,sats,temp,hum,press,volt\n");
    f.close();
}

std::vector<String> Flash::listFlights()
{
    std::vector<String> flights;
    Dir dir = LittleFS.openDir("/");

    while (dir.next())
    {
        if (!dir.isDirectory())
        {
            String name = dir.fileName();
            // filter by filename pattern
            if (name.startsWith(FILENAME_PREFIX) && name.endsWith(FILENAME_SUFFIX))
            {
                flights.push_back(name);
            }
        }
    }

    return flights;
}

void Flash::formatFlightData()
{
    LittleFS.format(); // Format the filesystem
    DEBUG_PRINTLN("[Flash] Flight data formatted. Please perform a system reset to reinitialize.");
    initialized = false; // Reset initialized state
}

void Flash::read(String flight_filename)
{
    File f = LittleFS.open(flight_filename, "r");
    if (!f)
    {
        DEBUG_PRINTLN("[Flash] Failed to open file for reading.");
        return; // Exit if file cannot be opened
    }

    while (f.available())
    {
        String line = f.readStringUntil('\n');
        Serial.println(line);
    }

    f.close();
}