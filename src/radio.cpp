#include "radio.h"

SX1262 sx1262 = new Module(SX1262_CS_PIN, SX1262_DIO1_PIN, SX1262_RST_PIN, SX1262_BUSY_PIN);
volatile bool Radio::transmittedFlag = false;

Radio::Radio()
{
}

void Radio::begin()
{
    // Constructor implementation
    SPI.setTX(SX1262_MOSI_PIN);
    SPI.setRX(SX1262_MISO_PIN);
    SPI.setSCK(SX1262_SCK_PIN);
    SPI.setCS(SX1262_CS_PIN);

    // Initialize the radio module
    // No TCXO and we have XTAL, LDO regulator is used
    int16_t status = sx1262.begin(SX1262_FREQUENCY, SX1262_BANDWIDTH, SX1262_SPREADING_FACTOR, SX1262_CODING_RATE, SX1262_SYNC_WORD, SX1262_TX_POWER, SX1262_PREAMBLE_LENGTH, 0, true);

    if (status != RADIOLIB_ERR_NONE)
    {
        DEBUG_PRINT("[SX1262] Radio initialization failed with error: ");
        DEBUG_PRINTLN(status);
    }
    else
    {
        DEBUG_PRINTLN("[SX1262] Radio initialized successfully.");
    }

    // We use DIO2 as RF switch
    status = sx1262.setDio2AsRfSwitch();
    if (status != RADIOLIB_ERR_NONE)
    {
        DEBUG_PRINT(F("[SX1262] Failed to set DIO2 as RF switch! Status: "));
        DEBUG_PRINTLN(status);
    }
    else
    {
        DEBUG_PRINTLN(F("[SX1262] DIO2 set as RF switch successfully."));
    }

    // set the function that will be called
    // when packet transmission is finished
    sx1262.setPacketSentAction(Radio::setFlag);
}

void Radio::setFlag()
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

void Radio::update()
{
    if (transmittedFlag)
    {
        // Reset the flag
        transmittedFlag = false;

        if (transmissionState == RADIOLIB_ERR_NONE)
        {
            // packet was successfully sent
            DEBUG_PRINTLN(F("[SX1262] transmission finished!"));
        }
        else
        {
            DEBUG_PRINT(F("[SX1262] failed, code "));
            DEBUG_PRINTLN(transmissionState);
        }

        // clean up after transmission is finished
        // this will ensure transmitter is disabled,
        // RF switch is powered down etc.
        sx1262.finishTransmit();
        readyFlag = true; // set the radio as ready for next transmission
    }
}

void Radio::sendPacket(const uint8_t *data, size_t size)
{
    // Send the packet
    if (!transmittedFlag)
    {
        DEBUG_PRINTLN(F("[SX1262] Starting transmission..."));
        transmissionState = sx1262.startTransmit(data, size);
    }
    else
    {
        DEBUG_PRINTLN(F("[SX1262] Transmission already in progress."));
    }
}

void Radio::waitForCommand()
{
    bool commandReceived = false;
    while (!commandReceived)
    {
        DEBUG_PRINTLN(F("[SX1262] Waiting for incoming transmission ... "));
        String str;
        int state = sx1262.receive(str);

        if (state == RADIOLIB_ERR_NONE)
        {
            // packet was successfully received
            DEBUG_PRINTLN(F("[SX1262] Received Packet!"));
            if (str == "START")
            {
                DEBUG_PRINTLN(F("[SX1262] Command received!"));
                commandReceived = true;
            }
            else
            {
                DEBUG_PRINTLN(F("[SX1262] Unknown command received."));
            }
        }
    }
}