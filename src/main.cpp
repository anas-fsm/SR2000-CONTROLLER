#include <Arduino.h>
#include "fsm/fsm.h"
#include "input/serial_input.h"

FSM fsm;
SerialInputManager serialInput;

unsigned long bootStartTime = 0;
bool autoLonTriggered = false;
const unsigned long TIMEOUT_DURATION = 30000; // Set 10 Detik (10000 ms)

void setup() {
    Serial.begin(115200);
    serialInput.begin(115200);
    fsm.begin();

    // Buang data/sampah Serial yang masuk saat ESP32 pertama kali menyala
    while (Serial.available() > 0) {
        Serial.read();
    }

    bootStartTime = millis();

    Serial.println("\n[SYSTEM] Controller Ready");
    Serial.println("[NAV] Serial Commands Active: W (UP), S (DOWN), D/Enter (OK), A/Esc (BACK)");
    Serial.println("[TIMEOUT] Auto LON in 10s if no input...");
}

void loop() {
    // 1. Cek Timeout Boot (10 detik)
    if (!autoLonTriggered) {
        if (millis() - bootStartTime >= TIMEOUT_DURATION) {
            autoLonTriggered = true;
            Serial.println("\n[TIMEOUT] No input detected. Auto-executing LON mode (BTN_OK)...");
            fsm.handleButton(BTN_OK);
        }
    }

    // 2. Baca Serial Input Terminal
    SerialInputResult serialEvt = serialInput.update();

    // 3. Jika ada input nyata dari pengguna
    if (serialEvt.action != SERIAL_ACTION_NONE) {
        if (!autoLonTriggered) {
            autoLonTriggered = true;
            Serial.println("\n[TIMEOUT] Timer cancelled: Interrupted by user input.");
        }

        switch (serialEvt.action) {
            case SERIAL_ACTION_UP:
                Serial.println("[SERIAL] Pressed: UP (W)");
                fsm.handleButton(BTN_UP);
                break;

            case SERIAL_ACTION_DOWN:
                Serial.println("[SERIAL] Pressed: DOWN (S)");
                fsm.handleButton(BTN_DOWN);
                break;

            case SERIAL_ACTION_OK:
                Serial.println("[SERIAL] Pressed: OK (D/Enter)");
                fsm.handleButton(BTN_OK);
                break;

            case SERIAL_ACTION_BACK:
                Serial.println("[SERIAL] Pressed: BACK (A/Esc)");
                fsm.handleButton(BTN_BACK);
                break;

            default:
                break;
        }
    }

    // 4. Jalankan pembaruan FSM & Tombol Fisik
    fsm.update();
}



// #include <Arduino.h>
// #include "fsm/fsm.h"
// #include "input/serial_input.h"

// FSM fsm;
// SerialInputManager serialInput;

// void setup() {
//     Serial.begin(115200);
//     serialInput.begin(115200);
//     fsm.begin();

//     Serial.println("\n[SYSTEM] Controller Ready");
//     Serial.println("[NAV] Serial Commands Active: W (UP), S (DOWN), D/Enter (OK), A/Esc (BACK)");
// }

// void loop() {
//     // 1. Baca Serial Input Terminal
//     SerialInputResult serialEvt = serialInput.update();

//     // 2. Jika ada ketikan di Terminal Serial, teruskan ke handleButton FSM
//     if (serialEvt.action != SERIAL_ACTION_NONE) {
//         switch (serialEvt.action) {
//             case SERIAL_ACTION_UP:
//                 Serial.println("[SERIAL] Pressed: UP (W)");
//                 fsm.handleButton(BTN_UP);
//                 break;

//             case SERIAL_ACTION_DOWN:
//                 Serial.println("[SERIAL] Pressed: DOWN (S)");
//                 fsm.handleButton(BTN_DOWN);
//                 break;

//             case SERIAL_ACTION_OK:
//                 Serial.println("[SERIAL] Pressed: OK (D/Enter)");
//                 fsm.handleButton(BTN_OK);
//                 break;

//             case SERIAL_ACTION_BACK:
//                 Serial.println("[SERIAL] Pressed: BACK (A/Esc)");
//                 fsm.handleButton(BTN_BACK);
//                 break;

//             default:
//                 break;
//         }
//     }

//     // 3. Jalankan pembaruan FSM & Tombol Fisik
//     fsm.update();
// }