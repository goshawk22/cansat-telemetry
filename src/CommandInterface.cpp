#include "CommandInterface.h"

CommandInterface::CommandInterface() {}

void CommandInterface::begin(Flash *flashPTR)
{
    flash = flashPTR;
    if (!flash->isInitialized())
    {
        flash->begin();
    }
    running = true;
    printMenu();
}

void CommandInterface::loop()
{
    if (!running)
        return;

    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        input.trim();
        handleCommand(input);
    }
}

void CommandInterface::printMenu()
{
    Serial.println("=== Command Interface ===");
    Serial.println("1: List flights");
    Serial.println("2: Download flight data");
    Serial.println("3: Format flight data");
    Serial.println("exit: Exit Command Interface and continue setup");
    Serial.print("> ");
}

void CommandInterface::handleCommand(const String &cmd)
{
    if (cmd == "1")
    {
        std::vector<String> flights = flash->listFlights();
        if (flights.empty())
        {
            Serial.println("No flights found.");
        }
        else
        {
            Serial.println("Available flights:");
            for (const String &flight : flights)
            {
                Serial.println(flight);
            }
        }
    }
    else if (cmd == "2")
    {
        Serial.print("Enter flight number to download (e.g., 1) > ");
        // Wait for user input
        while (!Serial.available())
        {
            delay(10);
        }
        String flightNumber = Serial.readStringUntil('\n');
        flightNumber.trim();
        if (flightNumber.toInt() > 0)
        {
            std::vector<String> flights = flash->listFlights();
            if (flightNumber.toInt() <= flights.size())
            {
                flash->read(flights[flightNumber.toInt() - 1]);
            }
            else
            {
                Serial.println("Invalid flight number.");
            }
        }
        else
        {
            Serial.println("Invalid input. Please enter a valid flight number.");
        }
    }
    else if (cmd == "3")
    {
        Serial.println("Formatting flight data...");
        Serial.println("This will erase all flight data.");
        Serial.print("Are you sure? Type 'yes' to confirm > ");
        // Wait for user input
        while (!Serial.available())
        {
            delay(10);
        }
        String confirmation = Serial.readStringUntil('\n');
        confirmation.trim();
        if (confirmation.equalsIgnoreCase("yes"))
        {
            Serial.println("Flight data will be formatted.");
            flash->formatFlightData();
            Serial.println("Flight data formatted successfully. Please perform a system reset to reinitialize.");
        }
        else
        {
            Serial.println("Format cancelled.");
        }
    }
    else if (cmd == "exit")
    {
        Serial.println("Exiting command interface.");
        running = false;
        return;
    }
    else
    {
        Serial.println("Unknown command.");
    }
    printMenu();
}