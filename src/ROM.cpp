#include "ROM.hpp"



void ROM::store(u32 addr, u8 value) {
    if (addr < ROM_SIZE)
        mem[addr] = value;
}

u8 ROM::load(u32 addr) {
    if (addr < ROM_SIZE)
        return mem[addr];

    return 0;
}