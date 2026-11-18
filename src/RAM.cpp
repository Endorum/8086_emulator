#include "RAM.hpp"



void RAM::store(u32 addr, u8 value) {
    if (addr < RAM_SIZE)
        mem[addr] = value;
}

u8 RAM::load(u32 addr) {
    if (addr < RAM_SIZE)
        return mem[addr];

    return 0;
}