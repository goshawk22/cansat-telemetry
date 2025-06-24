#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <Arduino.h>
#include "Flash.h"

class CommandInterface
{
public:
    CommandInterface();
    void begin(Flash *flashPTR);
    void loop();
    bool isRunning() const
    {
        return running;
    }

private:
    bool running = false;
    Flash *flash;
    void printMenu();
    void handleCommand(const String &cmd);
};

#endif