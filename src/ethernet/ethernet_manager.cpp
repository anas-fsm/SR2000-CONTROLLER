#include "ethernet_manager.h"
#include "../config/pins.h"
#include "../config/constants.h"

static EthernetManager* instance = nullptr;

void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            DEBUG_PRINTLN("[ETH] Ethernet Started");
            ETH.setHostname("esp32-sr2000-ctrl");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            DEBUG_PRINTLN("[ETH] Ethernet Link Up");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            DEBUG_PRINT("[ETH] IP Acquired: ");
            DEBUG_PRINTLN(ETH.localIP());
            if (instance) instance->setConnectedState(true);
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            DEBUG_PRINTLN("[ETH] Ethernet Link Down");
            if (instance) instance->setConnectedState(false);
            break;
        case ARDUINO_EVENT_ETH_STOP:
            DEBUG_PRINTLN("[ETH] Ethernet Stopped");
            if (instance) instance->setConnectedState(false);
            break;
        default:
            break;
    }
}

EthernetManager::EthernetManager() : ethConnected(false) {
    instance = this;
}

bool EthernetManager::begin(const NetworkConfig &config) {
    WiFi.onEvent(WiFiEvent);
    
    if (!ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_LAN8720, ETH_CLK_MODE)) {
        DEBUG_PRINTLN("[ETH] LAN8720 Hardware Init Failed");
        return false;
    }

    ETH.config(config.controllerIP, config.gateway, config.subnet, config.dns);
    return true;
}

void EthernetManager::update() {
    // Handling non-blocking ETH monitoring jika dibutuhkan
}

bool EthernetManager::isConnected() {
    return ethConnected;
}

IPAddress EthernetManager::getLocalIP() {
    return ETH.localIP();
}

bool EthernetManager::isLinkUp() {
    return ETH.linkUp();
}

void EthernetManager::setConnectedState(bool state) {
    ethConnected = state;
}