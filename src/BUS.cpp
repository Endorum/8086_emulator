#include "BUS.hpp"

#include "DEVICE.hpp"
#include <format>

DEVICE* BUS::route(u32 addr) const {

    for ( DEVICE* dev : devices ) {
        u32 start = dev->get_start();
        u32 size  = dev->get_size();

        if (addr >= start) {
            u32 offset = addr - start; // relativ addr

            if (offset < size) {
                return dev;
            }

        }
    }

    return nullptr;

}

void BUS::add_device(DEVICE* dev) {
    devices.push_back(dev);
}

void BUS::store(u32 addr, u8 value) {
    DEVICE* dev = route(addr);

    if (!dev) {
        throw std::runtime_error( std::format( "STORE ACCESS FAULT: {:08X}\n", addr ) );
    }

    u32 rel_addr = addr - dev->get_start();
    dev->store(rel_addr, value);

}

u8 BUS::load(u32 addr) {
    DEVICE* dev = route(addr);

    if (!dev) {
        throw std::runtime_error( std::format( "LOAD ACCESS FAULT: {:08X}\n", addr ) );
    }

    u32 rel_addr = addr - dev->get_start();
    return dev->load(addr);
}