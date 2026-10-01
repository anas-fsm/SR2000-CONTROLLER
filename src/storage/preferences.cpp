#include "preferences.h"
#include "constants.h"

PreferencesManager::PreferencesManager() {}

bool PreferencesManager::begin() {
    return prefs.begin("ctrl_cfg", false);
}

bool PreferencesManager::loadConfig(NetworkConfig &config) {
    if (!prefs.getBool("valid", false)) {
        DEBUG_PRINTLN("[CONFIG] No NVS config found. Using Defaults.");
        config = DEFAULT_CONFIG;
        return false;
    }
    
    config.controllerIP = IPAddress(prefs.getUInt("ctrl_ip", (uint32_t)DEFAULT_CONFIG.controllerIP));
    config.subnet       = IPAddress(prefs.getUInt("subnet",  (uint32_t)DEFAULT_CONFIG.subnet));
    config.gateway      = IPAddress(prefs.getUInt("gateway", (uint32_t)DEFAULT_CONFIG.gateway));
    config.dns          = IPAddress(prefs.getUInt("dns",     (uint32_t)DEFAULT_CONFIG.dns));
    config.scannerIP    = IPAddress(prefs.getUInt("scan_ip", (uint32_t)DEFAULT_CONFIG.scannerIP));
    config.scannerPort  = prefs.getUShort("scan_port", DEFAULT_CONFIG.scannerPort);

    DEBUG_PRINTLN("[CONFIG] Loaded configuration successfully from NVS");
    return true;
}

bool PreferencesManager::saveConfig(const NetworkConfig &config) {
    prefs.putUInt("ctrl_ip", (uint32_t)config.controllerIP);
    prefs.putUInt("subnet",  (uint32_t)config.subnet);
    prefs.putUInt("gateway", (uint32_t)config.gateway);
    prefs.putUInt("dns",     (uint32_t)config.dns);
    prefs.putUInt("scan_ip", (uint32_t)config.scannerIP);
    prefs.putUShort("scan_port", config.scannerPort);
    prefs.putBool("valid", true);
    
    DEBUG_PRINTLN("[CONFIG] Configuration saved to NVS");
    return true;
}