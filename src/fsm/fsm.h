#ifndef FSM_H
#define FSM_H

#include "../config/config.h"
#include "../storage/preferences.h"
#include "../input/buttons.h"
#include "../display/lcd.h"
#include "../ethernet/ethernet_manager.h"
#include "../scanner/scanner_tcp.h"

enum SystemState {
    STATE_BOOT,
    STATE_MAIN_MENU,
    STATE_LON,
    STATE_LOFF,
    STATE_CONTROLLER_IP,
    STATE_SCANNER_IP,
    STATE_ETHERNET_STATUS,
    STATE_SCANNER_STATUS,
    STATE_ERROR
};

class FSM {
private:
    SystemState currentState;
    
    // Dependencies
    PreferencesManager prefMgr;
    ButtonManager btnMgr;
    LCDManager lcdMgr;
    EthernetManager ethMgr;
    ScannerTCP scannerTcp;
    NetworkConfig sysConfig;

    // Boot Variables
    int bootStep;
    unsigned long bootTimer;

    // Menu Variables
    int menuSelectedIdx;
    int menuTopIdx;

    // IP Editor Variables
    IPAddress tempEditIP;
    int activeOctet;
    bool confirmMode;
    bool saveChoice;

    // Error Message Variables
    char errorHeader[21];
    char errorLine1[21];
    char errorLine2[21];

    void changeState(SystemState newState);
    void handleBootState();
    void handleMainMenuState();
    void handleCommandState(const char* title, const char* cmd);
    void handleIPEditorState(bool isControllerIP);
    void handleStatusScreenState(bool isEthernet);

public:
    FSM();
    void begin();
    void update();

    // Handler untuk menerima input tombol eksternal (dari Serial Terminal / WASD)
    void handleButton(ButtonType btn); 
};

#endif // FSM_H