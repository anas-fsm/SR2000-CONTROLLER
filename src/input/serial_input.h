#ifndef SERIAL_INPUT_H
#define SERIAL_INPUT_H

#include <Arduino.h>

enum SerialAction {
    SERIAL_ACTION_NONE = 0,
    SERIAL_ACTION_UP,
    SERIAL_ACTION_DOWN,
    SERIAL_ACTION_OK,
    SERIAL_ACTION_BACK,
    SERIAL_ACTION_CHAR
};

struct SerialInputResult {
    SerialAction action;
    char character;
};

class SerialInputManager {
public:
    SerialInputManager();
    void begin(unsigned long baudRate = 115200);
    SerialInputResult update();
};

#endif // SERIAL_INPUT_H