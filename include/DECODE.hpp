#pragma once
#include <cstdint>
#include <vector>

enum class PREFIX : uint8_t {
    NONE   = 0x00,
    REP    = 0xF3,
    REPNE  = 0xF2,
    CS_OVR = 0x2E,
    DS_OVR = 0x3E,
    ES_OVR = 0x26,
    SS_OVR = 0x36,
    LOCK   = 0xF0
};

inline std::string strPrefix(PREFIX p) {
    switch (p) {
        case PREFIX::NONE: return "NONE";
        case PREFIX::REP: return "REP";
        case PREFIX::REPNE: return "REPNE";
        case PREFIX::CS_OVR: return "CS_OVR";
        case PREFIX::DS_OVR: return "DS_OVR";
        case PREFIX::ES_OVR: return "ES_OVR";
        case PREFIX::SS_OVR: return "SS_OVR";
        case PREFIX::LOCK: return "LOCK";
    }

    return "???";
}

struct Instruction {

    // in the order they came in
    std::vector<PREFIX> prefixes;

    

};

inline std::string strInstruction(const Instruction& instr) {
    std::string str;

    str += "prefixes: ";
    for ( int i = 0; i < instr.prefixes.size(); i++ ) {
        str += strPrefix( instr.prefixes.at(i) );
        str += (i + 1 < instr.prefixes.size()) ? ", " : "";
    }

    return str;
}

Instruction decode(const uint8_t (&buf)[16]);