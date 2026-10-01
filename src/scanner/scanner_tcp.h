#ifndef SCANNER_TCP_H
#define SCANNER_TCP_H

#include <WiFiClient.h>
#include <IPAddress.h>

enum ScannerTcpState {
    SCANNER_IDLE,
    SCANNER_CONNECTING,
    SCANNER_SENDING,
    SCANNER_WAIT_RESPONSE,
    SCANNER_SUCCESS,
    SCANNER_FAILED_CONNECT,
    SCANNER_FAILED_TIMEOUT,
    SCANNER_FAILED_RESPONSE
};

class ScannerTCP {
private:
    WiFiClient client;
    IPAddress targetIP;
    uint16_t targetPort;
    ScannerTcpState state;
    unsigned long stateTimer;
    String pendingCommand;
    String lastResponse;

public:
    ScannerTCP();
    void update();
    void executeCommand(IPAddress ip, uint16_t port, const char* cmd);
    ScannerTcpState getState();
    String getLastResponse();
    bool isConnected();
    void disconnect();
};

#endif // SCANNER_TCP_H