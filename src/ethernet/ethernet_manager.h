#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <ETH.h>
#include "../config/config.h"

class EthernetManager {
private:
    bool ethConnected;

public:
    EthernetManager();
    bool begin(const NetworkConfig &config);
    void update();
    bool isConnected();
    IPAddress getLocalIP();
    bool isLinkUp();
    void setConnectedState(bool state);
};

#endif // ETHERNET_MANAGER_H