#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum ButtonType {
    BTN_UP,
    BTN_DOWN,
    BTN_OK,
    BTN_BACK,
    BTN_COUNT
};

class ButtonManager {
private:
    struct Button {
        uint8_t pin;
        bool lastState;
        bool currentState;
        unsigned long lastDebounceTime;
        bool pressedEvent;
    };
    Button buttons[BTN_COUNT];

public:
    ButtonManager();
    void begin();
    void update();
    bool isPressed(ButtonType btn);
};

#endif // BUTTONS_H