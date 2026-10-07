#include "serial_input.h"

SerialInputManager::SerialInputManager() {}

void SerialInputManager::begin(unsigned long baudRate) {
    // Jalankan Serial jika belum diinisialisasi
    if (!Serial) {
        Serial.begin(baudRate);
    }
}

SerialInputResult SerialInputManager::update() {
    SerialInputResult result = { SERIAL_ACTION_NONE, 0 };

    if (Serial.available() > 0) {
        char c = Serial.read();

        // Handle Escape Sequences untuk Tombol Panah (Arrow Keys Terminal)
        if (c == 27) { // ASCII 27 = ESC
            if (Serial.available() >= 2) {
                char next1 = Serial.read();
                char next2 = Serial.read();
                if (next1 == '[') {
                    if (next2 == 'A') result.action = SERIAL_ACTION_UP;    // Arrow Up
                    else if (next2 == 'B') result.action = SERIAL_ACTION_DOWN;  // Arrow Down
                    else if (next2 == 'C') result.action = SERIAL_ACTION_OK;    // Arrow Right
                    else if (next2 == 'D') result.action = SERIAL_ACTION_BACK;  // Arrow Left
                    return result;
                }
            } else {
                result.action = SERIAL_ACTION_BACK; // Tekan ESC biasa = BACK
                return result;
            }
        }

        // Pemetaan Tombol WASD dan Control Keys
        switch (c) {
            case 'w':
            case 'W':
                result.action = SERIAL_ACTION_UP;
                break;

            case 's':
            case 'S':
                result.action = SERIAL_ACTION_DOWN;
                break;

            case '\r': // Enter
            case '\n':
            case 'd':
            case 'D':
            case ' ':  // Spasi
                result.action = SERIAL_ACTION_OK;
                break;

            case 'a':
            case 'A':
            case 8:    // Backspace ASCII
            case 127:  // Delete/Backspace ASCII
                result.action = SERIAL_ACTION_BACK;
                break;

            default:
                // Jika mengetik angka/karakter biasa
                if (c >= 32 && c <= 126) {
                    result.action = SERIAL_ACTION_CHAR;
                    result.character = c;
                }
                break;
        }
    }

    return result;
}