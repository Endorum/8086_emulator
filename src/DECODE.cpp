#include "DECODE.hpp"
#include <vector>



PREFIX getPrefix(uint8_t byte) {
    switch (byte) {
        case 0x26: case 0x36:
        case 0xF3: case 0x2E: case 0x3E:  case 0xF0:
            return static_cast<PREFIX>(byte);

        case 0xF1:
            return PREFIX::LOCK;

        default:
            return PREFIX::NONE;
    }
}





Instruction decode(const uint8_t (&buf)[16]) {

    Instruction instr;


    // 1. handle prefixes
    uint8_t position = 0;

    for ( ; position < 16; position++ ) {
        PREFIX p = getPrefix(buf[position]);

        if (p != PREFIX::NONE) {
            instr.prefixes.push_back(p);
        } else break;
    }

    // position now points at first "real" instruction byte




    return instr;
}