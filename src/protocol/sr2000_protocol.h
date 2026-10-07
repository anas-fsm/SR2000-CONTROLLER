#ifndef SR2000_PROTOCOL_H
#define SR2000_PROTOCOL_H

#include <Arduino.h>

class SR2000Protocol {
public:
    static const char* CMD_LON;
    static const char* CMD_LOFF;

    enum ParseResult {
        PARSE_OK,
        PARSE_ERROR,
        PARSE_INCOMPLETE
    };

    // Asumsi protokol SR-2000: Scanner merespons dengan "LON\r" atau "OK\r" saat berhasil menerima command
    static ParseResult parseResponse(const String &response, const char* expectedCmd);
};

#endif // SR2000_PROTOCOL_H