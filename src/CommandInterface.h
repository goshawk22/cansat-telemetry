#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <Arduino.h>
#ifdef TRANSMITTER
#include "Flash.h"
#else
#include "radio.h"
#endif

class CommandInterface
{
public:
    CommandInterface();
#ifdef TRANSMITTER
    void begin(Flash *flashPTR);
#else
    void begin(Radio *radioPTR);
#endif
    void loop();
    bool isRunning() const
    {
        return running;
    }

private:
    bool running = false;
#ifdef TRANSMITTER
    Flash *flash;
#else
    Radio radio;
#endif
    void printMenu();
    void handleCommand(const String &cmd);
};

#endif