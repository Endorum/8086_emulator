#include <iostream>
#include <stdio.h>

#include "DECODE.hpp"


int main() {

    uint8_t buf[16] = {0xF3, 0xF0, 0x56, 0x78};

    Instruction instr = decode(buf);

    std::cout << strInstruction(instr) << std::endl;;

    return 0;
}