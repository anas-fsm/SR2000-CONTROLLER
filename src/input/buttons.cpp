#include "buttons.h"
#include "pins.h"
#include "constants.h"

ButtonManager::ButtonManager() {
    buttons[BTN_UP]   = {PIN_BTN_UP, HIGH, HIGH, 0, false};
    buttons[BTN_DOWN] = {PIN_BTN_DOWN, HIGH, HIGH, 0, false};
    buttons[BTN_OK]   = {PIN_BTN_OK, HIGH, HIGH, 0, false};
    buttons[BTN_BACK] = {PIN_BTN_BACK, HIGH, HIGH, 0, false};
}

void ButtonManager::begin() {
    for (int i = 0; i < BTN_COUNT; i++) {
        pinMode(buttons[i].pin, INPUT_PULLUP);
    }
}

void ButtonManager::update() {
    unsigned long now = millis();
    for (int i = 0; i < BTN_COUNT; i++) {
        bool reading = digitalRead(buttons[i].pin);
        buttons[i].pressedEvent = false;

        if (reading != buttons[i].lastState) {
            buttons[i].lastDebounceTime = now;
        }

        if ((now - buttons[i].lastDebounceTime) > DEBOUNCE_DELAY_MS) {
            if (reading != buttons[i].currentState) {
                buttons[i].currentState = reading;
                if (buttons[i].currentState == LOW) { // ACTIVE LOW
                    buttons[i].pressedEvent = true;
                    DEBUG_PRINTF("[BUTTON] Pressed: %d\n", i);
                }
            }
        }
        buttons[i].lastState = reading;
    }
}

bool ButtonManager::isPressed(ButtonType btn) {
    if (btn >= 0 && btn < BTN_COUNT) {
        return buttons[btn].pressedEvent;
    }
    return false;
}