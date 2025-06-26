#include "CommandInterface.h"

CommandInterface::CommandInterface() {}

#ifdef TRANSMITTER
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
#else
void CommandInterface::begin(Radio *radioPTR)
{
    // Initialize the radio module
    radio = *radioPTR;

    if (!radio.hasInitialised())
    {
        radio.begin();
        if (!radio.hasInitialised())
        {
            DEBUG_PRINTLN("Radio initialization failed. Please check the connections.");
            return;
        }
    }

    // Start the command interface
    running = true;
    printMenu();
}
#endif

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
#ifdef TRANSMITTER
    Serial.println("=== Command Interface (Transmitter) ===");
    Serial.println("=== Command Interface ===");
    Serial.println("1: List flights");
    Serial.println("2: Download flight data");
    Serial.println("3: Format flight data");
    Serial.println("exit: Exit Command Interface and continue setup");
    Serial.print("> ");
#else
    Serial.println("=== Command Interface (Receiver) ===");
    Serial.println("1: Send START command");
    Serial.println("exit: Exit Command Interface and start listening for data");
    Serial.print("> ");
#endif
}

void CommandInterface::handleCommand(const String &cmd)
{
#ifdef TRANSMITTER
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
#else
    if (cmd == "1")
    {
        bool gotAck = false;
        uint8_t numReceivedPackets = 0;
        while (!gotAck)
        {
            Serial.println("Sending START command...");
            radio.sendPacket((const uint8_t *)"START", 5);
            while (!radio.isReady())
            {
                // Wait until the radio is ready to send the packet
                radio.update();
            }
            // Wait for the ACK from the transmitter
            if (radio.isReady())
            {
                uint8_t buffer[5];
                int state = radio.receivePacket(buffer, sizeof(buffer));
                if (state == RADIOLIB_ERR_NONE)
                {
                    if (strncmp((const char *)buffer, "ACK", 3) == 0)
                    {
                        Serial.println("Received ACK for START command.");
                        gotAck = true;
                        running = false; // Exit the command interface
                        return;
                    }
                    else
                    {
                        Serial.println("Received unknown packet.");
                        numReceivedPackets++;
                        if (numReceivedPackets >= 5)
                        {
                            Serial.println("Lots of unknown packets received, exiting command interface as we probably missed the ACK.");
                            running = false;
                            return;
                        }
                    }
                }
                else
                {
                    Serial.print("Failed to receive packet, error code: ");
                    Serial.println(state);
                }
            }
            delay(1000); // Wait before sending again
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
#endif
    printMenu();
}