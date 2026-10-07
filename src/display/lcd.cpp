#include "lcd.h"
#include "../config/pins.h"
#include <Wire.h>

LCDManager::LCDManager() : lcd(0x27, 20, 4) {}

void LCDManager::begin() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void LCDManager::clear() {
    lcd.clear();
}

void LCDManager::showBootingScreen() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.print("  SR2000 CONTROLLER ");
    lcd.setCursor(0, 2);
    lcd.print("   System Starting  ");
    lcd.setCursor(0, 3);
    lcd.print("====================");
}

void LCDManager::showBootStep(const char* stepName) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print(stepName);
}

void LCDManager::showMainMenu(const char* items[], int itemCount, int topIdx, int selectedIdx) {
    lcd.clear();
    for (int i = 0; i < 4; i++) {
        int itemIndex = topIdx + i;
        if (itemIndex < itemCount) {
            lcd.setCursor(0, i);
            if (itemIndex == selectedIdx) {
                lcd.print(">");
            } else {
                lcd.print(" ");
            }
            lcd.print(items[itemIndex]);
        }
    }
}

void LCDManager::showCommandScreen(const char* title, const char* status, const char* line1, const char* line2) {
    lcd.clear();
    
    // Header
    lcd.setCursor(0, 0);
    lcd.print("[ ");
    lcd.print(title);
    lcd.print(" ]");

    // Status Line
    lcd.setCursor(0, 1);
    lcd.print(status);

    // Details Line 1 & 2
    lcd.setCursor(0, 2);
    lcd.print(line1);

    lcd.setCursor(0, 3);
    lcd.print(line2);
}

void LCDManager::showIPEditor(const char* title, IPAddress ip, int activeOctet, bool confirmMode, bool saveChoice) {
    lcd.clear();
    
    // Line 0: Header
    lcd.setCursor(0, 0);
    lcd.print(title);

    if (!confirmMode) {
        // Line 1: Tampilkan IP Address
        lcd.setCursor(0, 1);
        char ipBuf[21];
        snprintf(ipBuf, sizeof(ipBuf), "%3d.%3d.%3d.%3d", ip[0], ip[1], ip[2], ip[3]);
        lcd.print(ipBuf);

        // Line 2: Marker / Kursor penyunting octet
        lcd.setCursor(0, 2);
        int colOffset = activeOctet * 4;
        for (int i = 0; i < colOffset; i++) lcd.print(" ");
        lcd.print("^^^");

        // Line 3: Petunjuk Navigasi
        lcd.setCursor(0, 3);
        lcd.print("[UP/DN] Edit [OK] Next");
    } else {
        // Mode Konfirmasi Simpan
        lcd.setCursor(0, 1);
        lcd.print("Save Changes?");

        lcd.setCursor(0, 2);
        if (saveChoice) {
            lcd.print("> YES    NO ");
        } else {
            lcd.print("  YES  > NO ");
        }

        lcd.setCursor(0, 3);
        lcd.print("[OK] Select");
    }
}

void LCDManager::showEthernetStatus(const char* linkStatus, const char* ipStr, const char* connStatus) {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("=== ETH STATUS ===");

    lcd.setCursor(0, 1);
    lcd.print("Link : ");
    lcd.print(linkStatus);

    lcd.setCursor(0, 2);
    lcd.print("IP   : ");
    lcd.print(ipStr);

    lcd.setCursor(0, 3);
    lcd.print("State: ");
    lcd.print(connStatus);
}

void LCDManager::showScannerStatus(const char* ipStr, uint16_t port, const char* connStatus) {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("== SCANNER STATUS ==");

    lcd.setCursor(0, 1);
    lcd.print("IP   : ");
    lcd.print(ipStr);

    lcd.setCursor(0, 2);
    lcd.print("Port : ");
    lcd.print(port);

    lcd.setCursor(0, 3);
    lcd.print("TCP  : ");
    lcd.print(connStatus);
}

void LCDManager::showError(const char* header, const char* line1, const char* line2) {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("!!! ");
    lcd.print(header);
    lcd.print(" !!!");

    lcd.setCursor(0, 1);
    lcd.print(line1);

    lcd.setCursor(0, 2);
    lcd.print(line2);

    lcd.setCursor(0, 3);
    lcd.print("Press BACK to Return");
}