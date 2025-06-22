#include "radio.h"

SX1262 sx1262 = new Module(SX1262_CS_PIN, SX1262_DIO1_PIN, SX1262_RST_PIN, SX1262_BUSY_PIN);
volatile bool Radio::flag = false;

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
    flag = true;
}

void Radio::update()
{
    if (flag)
    {
        // Reset the flag
        flag = false;

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
    if (!flag)
    {
        DEBUG_PRINTLN(F("[SX1262] Starting transmission..."));
        transmissionState = sx1262.startTransmit(data, size);
    }
    else
    {
        DEBUG_PRINTLN(F("[SX1262] Transmission already in progress."));
    }
}

void Radio::receivePacket(uint8_t *buffer, size_t size)
{
    bool received = false;
    int state = sx1262.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        DEBUG_PRINTLN(F("[SX1262] Started Receiving..."));
    } else {
        DEBUG_PRINT(F("failed, code "));
        DEBUG_PRINTLN(state);
    }

    while (!received)
    {
        if (flag) {
            received = true; // we received a packet
            flag = false; // reset the flag
            int state = sx1262.readData(buffer, size);
            if (state == RADIOLIB_ERR_NONE)
            {
                // packet was successfully received
                DEBUG_PRINTLN(F("[SX1262] Received Packet!"));
                // Get the RSSI, SNR and frequency error
                rssi = sx1262.getRSSI();
                snr = sx1262.getSNR();
                frequencyError = sx1262.getFrequencyError();
            }
            else if (state == RADIOLIB_ERR_CRC_MISMATCH)
            {
                // packet was received, but is malformed
                DEBUG_PRINTLN(F("CRC error!"));
            }
            else
            {
                // some other error occurred
                DEBUG_PRINT(F("[SX1262] Error receiving packet: "));
                DEBUG_PRINTLN(state);
            }
        }
    }
}