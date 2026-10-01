#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_I2C.h>
#include <IPAddress.h>

class LCDManager {
private:
    LiquidCrystal_I2C lcd;

public:
    LCDManager();
    void begin();
    void clear();

    // Booting & Startup
    void showBootingScreen();
    void showBootStep(const char* stepName);

    // Menu Navigation
    void showMainMenu(const char* items[], int itemCount, int topIdx, int selectedIdx);

    // Command Status Execution
    void showCommandScreen(const char* title, const char* status, const char* line1, const char* line2);

    // IP Editor Interface
    void showIPEditor(const char* title, IPAddress ip, int activeOctet, bool confirmMode, bool saveChoice);

    // Status Monitors
    void showEthernetStatus(const char* linkStatus, const char* ipStr, const char* connStatus);
    void showScannerStatus(const char* ipStr, uint16_t port, const char* connStatus);

    // Error Alert Display
    void showError(const char* header, const char* line1, const char* line2);
};

#endif // LCD_H