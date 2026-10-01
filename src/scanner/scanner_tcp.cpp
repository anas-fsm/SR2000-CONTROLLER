#include "scanner_tcp.h"
#include "sr2000_protocol.h"
#include "constants.h"

ScannerTCP::ScannerTCP() : state(SCANNER_IDLE), stateTimer(0) {}

void ScannerTCP::executeCommand(IPAddress ip, uint16_t port, const char* cmd) {
    targetIP = ip;
    targetPort = port;
    pendingCommand = String(cmd);
    lastResponse = "";
    state = SCANNER_CONNECTING;
    stateTimer = millis();
    DEBUG_PRINTF("[TCP] Executing command %s to %s:%d\n", cmd, ip.toString().c_str(), port);
}

void ScannerTCP::update() {
    switch (state) {
        case SCANNER_IDLE:
            break;

        case SCANNER_CONNECTING:
            if (!client.connected()) {
                if (client.connect(targetIP, targetPort)) {
                    DEBUG_PRINTLN("[TCP] Connected to Scanner");
                    state = SCANNER_SENDING;
                } else if (millis() - stateTimer > TCP_TIMEOUT_MS) {
                    DEBUG_PRINTLN("[TCP] Connection Timeout");
                    state = SCANNER_FAILED_CONNECT;
                }
            } else {
                state = SCANNER_SENDING;
            }
            break;

        case SCANNER_SENDING:
            client.print(pendingCommand);
            DEBUG_PRINTF("[SR2000] TX: %s", pendingCommand.c_str());
            stateTimer = millis();
            state = SCANNER_WAIT_RESPONSE;
            break;

        case SCANNER_WAIT_RESPONSE:
            if (client.available()) {
                lastResponse = client.readStringUntil('\r');
                DEBUG_PRINTF("[SR2000] RX: %s\n", lastResponse.c_str());
                
                SR2000Protocol::ParseResult res = SR2000Protocol::parseResponse(lastResponse, pendingCommand.c_str());
                if (res == SR2000Protocol::PARSE_OK) {
                    state = SCANNER_SUCCESS;
                } else {
                    state = SCANNER_FAILED_RESPONSE;
                }
            } else if (millis() - stateTimer > TCP_TIMEOUT_MS) {
                DEBUG_PRINTLN("[TCP] Response Timeout");
                state = SCANNER_FAILED_TIMEOUT;
            }
            break;

        case SCANNER_SUCCESS:
        case SCANNER_FAILED_CONNECT:
        case SCANNER_FAILED_TIMEOUT:
        case SCANNER_FAILED_RESPONSE:
            disconnect();
            break;
    }
}

ScannerTcpState ScannerTCP::getState() {
    return state;
}

String ScannerTCP::getLastResponse() {
    return lastResponse;
}

bool ScannerTCP::isConnected() {
    return client.connected();
}

void ScannerTCP::disconnect() {
    if (client.connected()) {
        client.stop();
        DEBUG_PRINTLN("[TCP] Disconnected");
    }
}