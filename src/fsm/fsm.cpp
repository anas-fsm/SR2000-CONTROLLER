#include "fsm.h"
#include "../protocol/sr2000_protocol.h"
#include "../config/constants.h"

const char* MAIN_MENU_ITEMS[] = {
    "1. LON",
    "2. LOFF",
    "3. Controller IP",
    "4. Scanner IP",
    "5. Ethernet Status",
    "6. Scanner Status"
};
const int MAIN_MENU_COUNT = 6;

FSM::FSM() : currentState(STATE_BOOT), bootStep(0), bootTimer(0), menuSelectedIdx(0), menuTopIdx(0) {}

void FSM::begin() {
    lcdMgr.begin();
    btnMgr.begin();
    prefMgr.begin();
    lcdMgr.showBootingScreen();
    bootTimer = millis();
}

void FSM::changeState(SystemState newState) {
    DEBUG_PRINTF("[FSM] State Change: %d -> %d\n", currentState, newState);
    currentState = newState;

    switch (currentState) {
        case STATE_MAIN_MENU:
            lcdMgr.showMainMenu(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, menuTopIdx, menuSelectedIdx);
            break;
            
        case STATE_CONTROLLER_IP:
            tempEditIP = sysConfig.controllerIP;
            activeOctet = 0;
            confirmMode = false;
            saveChoice = true;
            lcdMgr.showIPEditor("Controller IP", tempEditIP, activeOctet, confirmMode, saveChoice);
            break;

        case STATE_SCANNER_IP:
            tempEditIP = sysConfig.scannerIP;
            activeOctet = 0;
            confirmMode = false;
            saveChoice = true;
            lcdMgr.showIPEditor("Scanner IP", tempEditIP, activeOctet, confirmMode, saveChoice);
            break;

        // PERBAIKAN: Gambar ke LCD CUKUP 1 KALI saat pertama masuk State
        case STATE_ETHERNET_STATUS:
            lcdMgr.showEthernetStatus(
                ethMgr.isLinkUp() ? "LINK UP" : "LINK DOWN",
                ethMgr.getLocalIP().toString().c_str(),
                ethMgr.isConnected() ? "CONNECTED" : "DISCONNECTED"
            );
            break;

        case STATE_SCANNER_STATUS:
            lcdMgr.showScannerStatus(
                sysConfig.scannerIP.toString().c_str(),
                sysConfig.scannerPort,
                scannerTcp.isConnected() ? "CONNECTED" : "DISCONNECTED"
            );
            break;

        case STATE_ERROR:
            lcdMgr.showError(errorHeader, errorLine1, errorLine2);
            break;

        default:
            break;
    }
}

void FSM::update() {
    btnMgr.update();
    ethMgr.update();
    scannerTcp.update();

    switch (currentState) {
        case STATE_BOOT:
            handleBootState();
            break;

        case STATE_MAIN_MENU:
            handleMainMenuState();
            break;

        case STATE_LON:
            handleCommandState("LON COMMAND", SR2000Protocol::CMD_LON);
            break;

        case STATE_LOFF:
            handleCommandState("LOFF COMMAND", SR2000Protocol::CMD_LOFF);
            break;

        case STATE_CONTROLLER_IP:
            handleIPEditorState(true);
            break;

        case STATE_SCANNER_IP:
            handleIPEditorState(false);
            break;

        case STATE_ETHERNET_STATUS:
            handleStatusScreenState(true);
            break;

        case STATE_SCANNER_STATUS:
            handleStatusScreenState(false);
            break;

        case STATE_ERROR:
            if (btnMgr.isPressed(BTN_BACK)) {
                changeState(STATE_MAIN_MENU);
            }
            break;
    }
}

void FSM::handleBootState() {
    if (millis() - bootTimer > BOOT_STEP_MS) {
        bootTimer = millis();
        bootStep++;

        switch (bootStep) {
            case 1:
                lcdMgr.showBootStep("Load Config...");
                prefMgr.loadConfig(sysConfig);
                break;

            case 2:
                lcdMgr.showBootStep("Ethernet Init...");
                if (!ethMgr.begin(sysConfig)) {
                    snprintf(errorHeader, sizeof(errorHeader), "ETH INIT FAIL");
                    snprintf(errorLine1, sizeof(errorLine1), "Check Hardware");
                    snprintf(errorLine2, sizeof(errorLine2), "LAN8720 Error");
                    changeState(STATE_ERROR);
                }
                break;

            case 3:
                lcdMgr.showBootStep("Network Wait...");
                break;

            case 4:
                lcdMgr.showBootStep("Ready!");
                break;

            case 5:
                changeState(STATE_MAIN_MENU);
                break;
        }
    }
}

void FSM::handleMainMenuState() {
    if (btnMgr.isPressed(BTN_DOWN)) {
        if (menuSelectedIdx < MAIN_MENU_COUNT - 1) {
            menuSelectedIdx++;
            if (menuSelectedIdx >= menuTopIdx + 4) menuTopIdx++;
            lcdMgr.showMainMenu(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, menuTopIdx, menuSelectedIdx);
        }
    } else if (btnMgr.isPressed(BTN_UP)) {
        if (menuSelectedIdx > 0) {
            menuSelectedIdx--;
            if (menuSelectedIdx < menuTopIdx) menuTopIdx--;
            lcdMgr.showMainMenu(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, menuTopIdx, menuSelectedIdx);
        }
    } else if (btnMgr.isPressed(BTN_OK)) {
        switch (menuSelectedIdx) {
            case 0: changeState(STATE_LON); break;
            case 1: changeState(STATE_LOFF); break;
            case 2: changeState(STATE_CONTROLLER_IP); break;
            case 3: changeState(STATE_SCANNER_IP); break;
            case 4: changeState(STATE_ETHERNET_STATUS); break;
            case 5: changeState(STATE_SCANNER_STATUS); break;
        }
    }
}

void FSM::handleCommandState(const char* title, const char* cmd) {
    static bool cmdTriggered = false;

    if (!cmdTriggered) {
        if (!ethMgr.isConnected()) {
            snprintf(errorHeader, sizeof(errorHeader), "ETHERNET DOWN");
            snprintf(errorLine1, sizeof(errorLine1), "LAN Cable Disconn");
            snprintf(errorLine2, sizeof(errorLine2), "Check Network");
            cmdTriggered = false;
            changeState(STATE_ERROR);
            return;
        }

        lcdMgr.showCommandScreen(title, "Sending command...", "", "");
        scannerTcp.executeCommand(sysConfig.scannerIP, sysConfig.scannerPort, cmd);
        cmdTriggered = true;
    }

    ScannerTcpState st = scannerTcp.getState();
    if (st == SCANNER_SUCCESS) {
        lcdMgr.showCommandScreen(title, "COMMAND SUCCESS", "Scanner: OK", "TCP: CONNECTED");
        if (btnMgr.isPressed(BTN_BACK) || btnMgr.isPressed(BTN_OK)) {
            cmdTriggered = false;
            changeState(STATE_MAIN_MENU);
        }
    } else if (st == SCANNER_FAILED_CONNECT) {
        cmdTriggered = false;
        snprintf(errorHeader, sizeof(errorHeader), "TCP FAIL");
        snprintf(errorLine1, sizeof(errorLine1), "Cannot connect");
        snprintf(errorLine2, sizeof(errorLine2), "Check Scanner IP");
        changeState(STATE_ERROR);
    } else if (st == SCANNER_FAILED_TIMEOUT) {
        cmdTriggered = false;
        snprintf(errorHeader, sizeof(errorHeader), "TIMEOUT");
        snprintf(errorLine1, sizeof(errorLine1), "No response from");
        snprintf(errorLine2, sizeof(errorLine2), "Scanner device");
        changeState(STATE_ERROR);
    } else if (st == SCANNER_FAILED_RESPONSE) {
        cmdTriggered = false;
        snprintf(errorHeader, sizeof(errorHeader), "CMD ERROR");
        snprintf(errorLine1, sizeof(errorLine1), "Scanner returned");
        snprintf(errorLine2, sizeof(errorLine2), "Error status");
        changeState(STATE_ERROR);
    }

    if (btnMgr.isPressed(BTN_BACK)) {
        cmdTriggered = false;
        scannerTcp.disconnect();
        changeState(STATE_MAIN_MENU);
    }
}

void FSM::handleIPEditorState(bool isControllerIP) {
    const char* title = isControllerIP ? "Controller IP" : "Scanner IP";

    if (!confirmMode) {
        if (btnMgr.isPressed(BTN_UP)) {
            if (tempEditIP[activeOctet] < 255) tempEditIP[activeOctet]++;
            lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
        } else if (btnMgr.isPressed(BTN_DOWN)) {
            if (tempEditIP[activeOctet] > 0) tempEditIP[activeOctet]--;
            lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
        } else if (btnMgr.isPressed(BTN_OK)) {
            if (activeOctet < 3) {
                activeOctet++;
            } else {
                confirmMode = true;
                saveChoice = true;
            }
            lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
        } else if (btnMgr.isPressed(BTN_BACK)) {
            if (activeOctet > 0) {
                activeOctet--;
                lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
            } else {
                changeState(STATE_MAIN_MENU);
            }
        }
    } else {
        if (btnMgr.isPressed(BTN_UP) || btnMgr.isPressed(BTN_DOWN)) {
            saveChoice = !saveChoice;
            lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
        } else if (btnMgr.isPressed(BTN_OK)) {
            if (saveChoice) {
                if (isControllerIP) {
                    sysConfig.controllerIP = tempEditIP;
                } else {
                    sysConfig.scannerIP = tempEditIP;
                }
                prefMgr.saveConfig(sysConfig);
                
                if (isControllerIP) {
                    ethMgr.begin(sysConfig);
                }
            }
            changeState(STATE_MAIN_MENU);
        } else if (btnMgr.isPressed(BTN_BACK)) {
            confirmMode = false;
            lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
        }
    }
}

void FSM::handleStatusScreenState(bool isEthernet) {
    // PERBAIKAN: Gunakan BTN_BACK saja untuk keluar agar Sinyal BTN_OK dari Menu Utama tidak memicu keluar otomatis
    if (btnMgr.isPressed(BTN_BACK)) {
        changeState(STATE_MAIN_MENU);
    }
}

// =========================================================================
// HANDLER TOMBOL EKSTERNAL (SERIAL / WASD)
// =========================================================================
void FSM::handleButton(ButtonType btn) {
    switch (currentState) {
        case STATE_MAIN_MENU:
            if (btn == BTN_DOWN) {
                if (menuSelectedIdx < MAIN_MENU_COUNT - 1) {
                    menuSelectedIdx++;
                    if (menuSelectedIdx >= menuTopIdx + 4) menuTopIdx++;
                    lcdMgr.showMainMenu(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, menuTopIdx, menuSelectedIdx);
                }
            } else if (btn == BTN_UP) {
                if (menuSelectedIdx > 0) {
                    menuSelectedIdx--;
                    if (menuSelectedIdx < menuTopIdx) menuTopIdx--;
                    lcdMgr.showMainMenu(MAIN_MENU_ITEMS, MAIN_MENU_COUNT, menuTopIdx, menuSelectedIdx);
                }
            } else if (btn == BTN_OK) {
                switch (menuSelectedIdx) {
                    case 0: changeState(STATE_LON); break;
                    case 1: changeState(STATE_LOFF); break;
                    case 2: changeState(STATE_CONTROLLER_IP); break;
                    case 3: changeState(STATE_SCANNER_IP); break;
                    case 4: changeState(STATE_ETHERNET_STATUS); break;
                    case 5: changeState(STATE_SCANNER_STATUS); break;
                }
            }
            break;

        case STATE_CONTROLLER_IP:
        case STATE_SCANNER_IP: {
            bool isControllerIP = (currentState == STATE_CONTROLLER_IP);
            const char* title = isControllerIP ? "Controller IP" : "Scanner IP";

            if (!confirmMode) {
                if (btn == BTN_UP) {
                    if (tempEditIP[activeOctet] < 255) tempEditIP[activeOctet]++;
                    lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                } else if (btn == BTN_DOWN) {
                    if (tempEditIP[activeOctet] > 0) tempEditIP[activeOctet]--;
                    lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                } else if (btn == BTN_OK) {
                    if (activeOctet < 3) {
                        activeOctet++;
                    } else {
                        confirmMode = true;
                        saveChoice = true;
                    }
                    lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                } else if (btn == BTN_BACK) {
                    if (activeOctet > 0) {
                        activeOctet--;
                        lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                    } else {
                        changeState(STATE_MAIN_MENU);
                    }
                }
            } else {
                if (btn == BTN_UP || btn == BTN_DOWN) {
                    saveChoice = !saveChoice;
                    lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                } else if (btn == BTN_OK) {
                    if (saveChoice) {
                        if (isControllerIP) {
                            sysConfig.controllerIP = tempEditIP;
                        } else {
                            sysConfig.scannerIP = tempEditIP;
                        }
                        prefMgr.saveConfig(sysConfig);
                        
                        if (isControllerIP) {
                            ethMgr.begin(sysConfig);
                        }
                    }
                    changeState(STATE_MAIN_MENU);
                } else if (btn == BTN_BACK) {
                    confirmMode = false;
                    lcdMgr.showIPEditor(title, tempEditIP, activeOctet, confirmMode, saveChoice);
                }
            }
            break;
        }

        case STATE_ETHERNET_STATUS:
        case STATE_SCANNER_STATUS:
            // PERBAIKAN: Keluar dari menu status hanya jika menekan tombol BACK (A/Esc)
            if (btn == BTN_BACK) {
                changeState(STATE_MAIN_MENU);
            }
            break;

        case STATE_LON:
        case STATE_LOFF:
        case STATE_ERROR:
            if (btn == BTN_BACK || btn == BTN_OK) {
                changeState(STATE_MAIN_MENU);
            }
            break;

        default:
            break;
    }
}