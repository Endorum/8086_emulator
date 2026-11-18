#pragma once


#include "DEFS.hpp"

class DEVICE {
  public:
    explicit DEVICE(u32 st, u32 sz) : start(st), size(sz) {}

    virtual ~DEVICE() = default;

    virtual void store(u32 addr, u8 value) = 0;
    virtual u8 load(u32 addr) = 0;

    u32 get_start() const { return start; }
    u32 get_size() const { return size; }


  private:
    u32 start;
    u32 size;
};