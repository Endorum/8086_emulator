#pragma once
#include "DEVICE.hpp"



class ROM : public DEVICE {
  public:
    explicit ROM() : DEVICE(ROM_START, ROM_SIZE) {
        mem = new u8[ROM_SIZE];
    }

    void store(u32 addr, u8 value) override;
    u8 load(u32 addr) override;

  private:
    u8* mem;
};