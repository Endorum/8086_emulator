#include <iostream>

#include "CPU.hpp"
#include "toString.hpp"


void CPU::reg_store(Register reg, u16 value) {
    switch (reg) {
        default: return;
        case Register::NONE: return;

        // normal
        case Register::AX: ax = value; return;
        case Register::BX: bx = value; return;
        case Register::CX: cx = value; return;
        case Register::DX: dx = value; return;
        case Register::SP: sp = value; return;
        case Register::BP: bp = value; return;
        case Register::SI: si = value; return;
        case Register::DI: di = value; return;

        // high
        case Register::AH: ax = (ax | 0x00FF) | (value << 8); return;
        case Register::BH: bx = (bx | 0x00FF) | (value << 8); return;
        case Register::CH: cx = (cx | 0x00FF) | (value << 8); return;
        case Register::DH: dx = (dx | 0x00FF) | (value << 8); return;

        // low
        case Register::AL: ax = (ax | 0xFF00) | (u8)(value); return;
        case Register::BL: bx = (bx | 0xFF00) | (u8)(value); return;
        case Register::CL: cx = (cx | 0xFF00) | (u8)(value); return;
        case Register::DL: dx = (dx | 0xFF00) | (u8)(value); return;

        // segments
        case Register::ES: es = value; return;
        case Register::CS: cs = value; return;
        case Register::SS: ss = value; return;
        case Register::DS: ds = value; return;
        
    }
}

u16 CPU::reg_load(Register reg) {
    switch (reg) {
        default: return 0;
        case Register::NONE: return 0;

        // normal
        case Register::AX: return ax;
        case Register::BX: return bx;
        case Register::CX: return cx;
        case Register::DX: return dx;
        case Register::SP: return sp;
        case Register::BP: return bp;
        case Register::SI: return si;
        case Register::DI: return di;

        // high
        case Register::AH: return (ax & 0xFF00);
        case Register::BH: return (bx & 0xFF00);
        case Register::CH: return (cx & 0xFF00);
        case Register::DH: return (dx & 0xFF00);

        // low
        case Register::AL: return (ax & 0x00FF);
        case Register::BL: return (bx & 0x00FF);
        case Register::CL: return (cx & 0x00FF);
        case Register::DL: return (dx & 0x00FF);

        // segments
        case Register::ES: return es;
        case Register::CS: return cs;
        case Register::SS: return ss;
        case Register::DS: return ds;
        
    }
}

void CPU::mem_store(u32 addr, u8 value) {
    bus->store(addr, value);
}

u8 CPU::mem_load(u32 addr) {
    return bus->load(addr);
}

void CPU::mem_store8(u16 segment, u16 offset, u8 value) {
    u32 addr = (segment << 4) + offset;
    mem_store(addr, value);
}

void CPU::mem_store16(u16 segment, u16 offset, u16 value) {
    mem_store8(segment, offset, value); // lo
    mem_store8(segment, offset + 1, (value << 8)); // hi
}

u8 CPU::mem_load8(u16 segment, u16 offset) {
    u32 addr = (segment << 4) + offset;
    return mem_load(addr);
}

u16 CPU::mem_load16(u16 segment, u16 offset) {
    u8 lo = mem_load8(segment, offset);
    u8 hi = mem_load8(segment, offset + 1);

    return lo | ((u16)(hi) << 8);
}


u32 CPU::eval_eabase(eaBase& base) {
    switch (base) {
        case eaBase::BX_SI: return reg_load(Register::BX) + reg_load(Register::SI);
        case eaBase::BX_DI: return reg_load(Register::BX) + reg_load(Register::DI);
        case eaBase::BP_SI: return reg_load(Register::BP) + reg_load(Register::SI);
        case eaBase::BP_DI: return reg_load(Register::BP) + reg_load(Register::DI);
        case eaBase::SI: return reg_load(Register::SI);
        case eaBase::DI: return reg_load(Register::DI);
        case eaBase::BP: return reg_load(Register::BP);
        case eaBase::BX: return reg_load(Register::BX);
        case eaBase::Direct: break;
    }
}

u16 CPU::segment_for(const Operand& op) {
    if (op.segOverride != Register::NONE)
        return reg_load(op.segOverride);

    switch (op.base) {
        case eaBase::BP_SI: case eaBase::BP_DI: case eaBase::BP:
            return ss;
        default:
            return ds;
    }
}

void CPU::store_dst(u16 value) {

    switch (instr.dst.kind) {
        default:
            std::cout << "TODO" << std::endl;
            return;

        case OpKind::REG:
            reg_store(instr.dst.reg, value);

        case OpKind::MEM:
            u16 offset  = eval_eabase(instr.src.base) + instr.src.disp;
            u16 segment = segment_for(instr.src);

            if (instr.w)
                mem_store16(segment, offset, value);
            else
                mem_store8(segment, offset, value);

    }
}

u16 CPU::load_src() {


    switch (instr.src.kind) {
        default:
            std::cout << "TODO" << std::endl;
            return 0;

        case OpKind::REG:
            return reg_load(instr.src.reg);

        case OpKind::MEM: {
            u16 offset  = eval_eabase(instr.src.base) + instr.src.disp;
            u16 segment = segment_for(instr.src);

            if (instr.w)
                return mem_load16(segment, offset);
            else
                return mem_load8(segment, offset);

        }

        case OpKind::IMM: {
            return instr.src.imm;
        }
        
    }
}




void CPU::reset() {
    ax = 0;
    bx = 0;
    cx = 0;
    dx = 0;

    sp = 0;
    bp = 0;
    si = 0;
    di = 0;

    cs = 0;
    ds = 0;
    es = 0;
    ss = 0;

    ip = 0;
    flags = 0;
}

void CPU::step() {

    // 1. fetch next 16 bytes to preload the decoder
    // and increment the instruction pointer with instruction length
    u8 buf[16];
    for ( uint16_t i = 0; i < 16; i++ ) {
        buf[i] = mem_load8(cs, ip + i);
    }

    instr = decode(buf, ip);
    ip += instr.length;
    std::cout << strInstruction(instr) << std::endl;

    //
    u16 src_value = load_src();
    store_dst(src_value);

    //


    // debug
    std::cout << "register: " << std::endl;
    std::cout << std::format( "AX: {:04X}, BX: {:04X}, CX: {:04X}, DX: {:04X}\n", ax, bx, cx, dx );
    std::cout << std::format( "SP: {:04X}, BP: {:04X}, SI: {:04X}, DI: {:04X}\n", sp, bp, si, di );
    std::cout << std::format( "CS: {:04X}, DS: {:04X}, ES: {:04X}, SS: {:04X}\n", cs, ds, es, ss );
    std::cout << std::format("IP: {:04X}\n", ip);
    std::cout << std::format("FLAGS: {:04X}\n", flags);

}


