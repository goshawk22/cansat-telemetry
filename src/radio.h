#ifndef RADIO_H
#define RADIO_H

#include "config.h"
#include <RadioLib.h>

class Radio
{
public:
    Radio();
    void begin();
    void update();
    void sendPacket(const uint8_t *data, size_t size);
    void receivePacket(uint8_t *buffer, size_t size);
    bool isReady() const
    {
        return readyFlag;
    }
    float getRSSI() const
    {
        return rssi;
    }
    float getSNR() const
    {
        return snr;
    }
    float getFrequencyError() const
    {
        return frequencyError;
    }

private:
    static void setFlag();

    // flag to indicate that a packet was sent
    static volatile bool flag;
    // flag to indicate that radio is ready for transmission
    bool readyFlag = true;
    int transmissionState = RADIOLIB_ERR_NONE;
    // Stats about received packets
    float rssi;
    float snr;
    float frequencyError;
};
#endif