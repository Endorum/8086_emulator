#pragma once
#include <vector>

#include "DEFS.hpp"
#include "DEVICE.hpp"



class BUS {
  public:
    BUS() = default;
    ~BUS() = default;

    void add_device(DEVICE* dev);

    DEVICE* route(u32 addr) const;
    void store(u32 addr, u8 value);
    u8 load(u32 addr);

  private:
    std::vector<DEVICE*> devices;

};