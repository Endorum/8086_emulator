#pragma once

#include "BUS.hpp"
#include "DECODE.hpp"
#include "DEFS.hpp"



class CPU {
  public:
    explicit CPU() = default;

    void reset();
    void step();

    void attach_bus(BUS* b) {
        bus = b;
    }

  private:

    BUS* bus;
    Instruction instr;

    // registers
    u16 ax, bx, cx, dx;
    u16 sp, bp, si, di;
    u16 cs, ds, es, ss;
    u16 ip;
    u16 flags;

    // register methods
    void reg_store(Register reg, u16 value);
    u16 reg_load(Register reg);

    // memory

    // physical memory
    void mem_store(u32 addr, u8 value);
    u8 mem_load(u32 addr);

    // normal
    void mem_store8(u16 segment, u16 offset, u8 value);
    void mem_store16(u16 segment, u16 offset, u16 value);
    u8 mem_load8(u16 segment, u16 offset);
    u16 mem_load16(u16 segment, u16 offset);

    // operands
    u32 eval_eabase(eaBase& base);
    u16 segment_for(const Operand& op);
    void store_dst(u16 value);
    u16 load_src();
    

};