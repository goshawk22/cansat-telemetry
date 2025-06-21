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
    bool isReady() const
    {
        return readyFlag;
    }
    void waitForCommand();

private:
    static void setFlag();

    // flag to indicate that a packet was sent
    static volatile bool transmittedFlag;
    // flag to indicate that radio is ready for transmission
    bool readyFlag = true;
    int transmissionState = RADIOLIB_ERR_NONE;
};
#endif