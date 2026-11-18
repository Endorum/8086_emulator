#pragma once
#include "DEVICE.hpp"



class RAM : public DEVICE {
  public:
    explicit RAM() : DEVICE(RAM_START, RAM_SIZE) {
        mem = new u8[RAM_SIZE];
    }

    void store(u32 addr, u8 value) override;
    u8 load(u32 addr) override;

  private:
    u8* mem;
};