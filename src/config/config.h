#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct NetworkConfig {
    IPAddress controllerIP;
    IPAddress subnet;
    IPAddress gateway;
    IPAddress dns;
    IPAddress scannerIP;
    uint16_t scannerPort;
};

// Default Configuration values
const NetworkConfig DEFAULT_CONFIG = {
    IPAddress(192, 168, 137, 50),   // Controller IP
    IPAddress(255, 255, 255, 0),    // Subnet
    IPAddress(192, 168, 137, 1),    // Gateway
    IPAddress(8, 8, 8, 8),          // DNS
    IPAddress(192, 168, 5, 194),    // Scanner IP
    9000                            // Scanner Port
};

#endif // CONFIG_H