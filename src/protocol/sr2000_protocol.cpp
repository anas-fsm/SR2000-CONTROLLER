#include "sr2000_protocol.h"

const char* SR2000Protocol::CMD_LON  = "LON\r";
const char* SR2000Protocol::CMD_LOFF = "LOFF\r";

SR2000Protocol::ParseResult SR2000Protocol::parseResponse(const String &response, const char* expectedCmd) {
    if (response.length() == 0) return PARSE_INCOMPLETE;
    
    // Keyence SR-2000 biasanya mengembalikan ACK berupa string command/OK disusul CR
    if (response.indexOf("OK") != -1 || response.indexOf("LON") != -1 || response.indexOf("LOFF") != -1) {
        return PARSE_OK;
    }
    
    if (response.indexOf("ER") != -1) {
        return PARSE_ERROR;
    }

    return PARSE_OK; // Default fallback apabila format custom
}