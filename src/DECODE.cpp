#include "DECODE.hpp"
#include <format>
#include <vector>

#include "DEFS.hpp"


PREFIX getPrefix(u8 byte) {
    switch (byte) {
        case 0x26: case 0x36:
        case 0xF3: case 0x2E: case 0x3E:  case 0xF0:
            return static_cast<PREFIX>(byte);

        case 0xF1:
            return PREFIX::LOCK;

        default:
            return PREFIX::NONE;
    }
}


Register solveRegister(u8 reg, bool w) {
    switch (reg) {
        case 0b000: return (w == true) ? Register::AX : Register::AL;
        case 0b001: return (w == true) ? Register::CX : Register::CL;
        case 0b010: return (w == true) ? Register::DX : Register::DL;
        case 0b011: return (w == true) ? Register::BX : Register::BL;
        case 0b100: return (w == true) ? Register::SP : Register::AH;
        case 0b101: return (w == true) ? Register::BP : Register::CH;
        case 0b110: return (w == true) ? Register::SI : Register::DH;
        case 0b111: return (w == true) ? Register::DI : Register::BH;
    }
    throw std::runtime_error( std::format( "invalid register index: 0x{:02X}\n", reg ) );
}

Register solveSegment(u8 sreg) {
    switch (sreg) {
        case 0b00: return Register::ES;
        case 0b01: return Register::CS;
        case 0b10: return Register::SS;
        case 0b11: return Register::DS;
    }
    throw std::runtime_error( std::format( "invalid segment register index: 0x{:02X}\n", sreg ) );
}

Operand makeReg(u8 index, bool w) {
    return Operand{.kind = OpKind::REG, .reg = solveRegister(index, w)};
}


Operand decodeRm(Instruction& instr) {
    if (instr.mod == 0b11) {
        return makeReg(instr.rm, instr.w);
    }

    Operand op;
    op.kind = OpKind::MEM;
    op.base = static_cast<eaBase>(instr.rm);

    if ( instr.mod == 0b00 && instr.rm == 0b110 ) {
        op.base = eaBase::Direct;
        op.disp = (i16)(instr.advance16());
    }

    else if (instr.mod == 0b01) {
        op.disp = (i8)(instr.advance()); // 8 bit disp
    }

    else if (instr.mod == 0b10) {
        op.disp = (i16)(instr.advance16()); // 16 bit disp
    }

    return op;

}

u16 solveImm(Instruction& instr, bool w) {
    if (w) return instr.advance16();
    return instr.advance();
}

Register segOverrideFromPrefixes(Instruction& instr) {
    for ( int i = 0; i < instr.prefixes.size(); i++ ) {
        if (instr.prefixes[i] == PREFIX::CS_OVR) return Register::CS;
        if (instr.prefixes[i] == PREFIX::DS_OVR) return Register::DS;
        if (instr.prefixes[i] == PREFIX::ES_OVR) return Register::ES;
        if (instr.prefixes[i] == PREFIX::SS_OVR) return Register::SS;
    }
    return Register::NONE;
}

Instruction decode(const u8* buf, u16 addr) {

    Instruction instr(buf, addr);


    // 1. handle prefixes
    instr.position = 0;

    while (instr.position < 16) {
        PREFIX p = getPrefix(instr.peek());
        if (p == PREFIX::NONE) break;
        instr.prefixes.push_back(p);
        instr.advance();
    }

    // position now points at first "real" instruction byte
    OpcodeInfo OPCODES[256] = {

        {Mnemonic::ADD, Encoding::RegRm},        // 00
        {Mnemonic::ADD, Encoding::RegRm},        // 01
        {Mnemonic::ADD, Encoding::RegRm},        // 02
        {Mnemonic::ADD, Encoding::RegRm},        // 03
        {Mnemonic::ADD, Encoding::AccImm},       // 04
        {Mnemonic::ADD, Encoding::AccImm},       // 05
        {Mnemonic::PUSH, Encoding::SReg},        // 06 PUSH ES
        {Mnemonic::POP, Encoding::SReg},         // 07 POP ES
        {Mnemonic::OR, Encoding::RegRm},         // 08
        {Mnemonic::OR, Encoding::RegRm},         // 09
        {Mnemonic::OR, Encoding::RegRm},         // 0A
        {Mnemonic::OR, Encoding::RegRm},         // 0B
        {Mnemonic::OR, Encoding::AccImm},        // 0C
        {Mnemonic::OR, Encoding::AccImm},        // 0D
        {Mnemonic::PUSH, Encoding::SReg},        // 0E PUSH CS
        {Mnemonic::POP, Encoding::SReg},         // 0F POP CS (undocumented, 8086 only)
        {Mnemonic::ADC, Encoding::RegRm},        // 10
        {Mnemonic::ADC, Encoding::RegRm},        // 11
        {Mnemonic::ADC, Encoding::RegRm},        // 12
        {Mnemonic::ADC, Encoding::RegRm},        // 13
        {Mnemonic::ADC, Encoding::AccImm},       // 14
        {Mnemonic::ADC, Encoding::AccImm},       // 15
        {Mnemonic::PUSH, Encoding::SReg},        // 16 PUSH SS
        {Mnemonic::POP, Encoding::SReg},         // 17 POP SS
        {Mnemonic::SBB, Encoding::RegRm},        // 18
        {Mnemonic::SBB, Encoding::RegRm},        // 19
        {Mnemonic::SBB, Encoding::RegRm},        // 1A
        {Mnemonic::SBB, Encoding::RegRm},        // 1B
        {Mnemonic::SBB, Encoding::AccImm},       // 1C
        {Mnemonic::SBB, Encoding::AccImm},       // 1D
        {Mnemonic::PUSH, Encoding::SReg},        // 1E PUSH DS
        {Mnemonic::POP, Encoding::SReg},         // 1F POP DS
        {Mnemonic::AND, Encoding::RegRm},        // 20
        {Mnemonic::AND, Encoding::RegRm},        // 21
        {Mnemonic::AND, Encoding::RegRm},        // 22
        {Mnemonic::AND, Encoding::RegRm},        // 23
        {Mnemonic::AND, Encoding::AccImm},       // 24
        {Mnemonic::AND, Encoding::AccImm},       // 25
        {Mnemonic::SEG, Encoding::Prefix},       // 26 ES:
        {Mnemonic::DAA, Encoding::None},         // 27
        {Mnemonic::SUB, Encoding::RegRm},        // 28
        {Mnemonic::SUB, Encoding::RegRm},        // 29
        {Mnemonic::SUB, Encoding::RegRm},        // 2A
        {Mnemonic::SUB, Encoding::RegRm},        // 2B
        {Mnemonic::SUB, Encoding::AccImm},       // 2C
        {Mnemonic::SUB, Encoding::AccImm},       // 2D
        {Mnemonic::SEG, Encoding::Prefix},       // 2E CS:
        {Mnemonic::DAS, Encoding::None},         // 2F
        {Mnemonic::XOR, Encoding::RegRm},        // 30
        {Mnemonic::XOR, Encoding::RegRm},        // 31
        {Mnemonic::XOR, Encoding::RegRm},        // 32
        {Mnemonic::XOR, Encoding::RegRm},        // 33
        {Mnemonic::XOR, Encoding::AccImm},       // 34
        {Mnemonic::XOR, Encoding::AccImm},       // 35
        {Mnemonic::SEG, Encoding::Prefix},       // 36 SS:
        {Mnemonic::AAA, Encoding::None},         // 37
        {Mnemonic::CMP, Encoding::RegRm},        // 38
        {Mnemonic::CMP, Encoding::RegRm},        // 39
        {Mnemonic::CMP, Encoding::RegRm},        // 3A
        {Mnemonic::CMP, Encoding::RegRm},        // 3B
        {Mnemonic::CMP, Encoding::AccImm},       // 3C
        {Mnemonic::CMP, Encoding::AccImm},       // 3D
        {Mnemonic::SEG, Encoding::Prefix},       // 3E DS:
        {Mnemonic::AAS, Encoding::None},         // 3F
        {Mnemonic::INC, Encoding::Reg16},        // 40 INC AX
        {Mnemonic::INC, Encoding::Reg16},        // 41 INC CX
        {Mnemonic::INC, Encoding::Reg16},        // 42 INC DX
        {Mnemonic::INC, Encoding::Reg16},        // 43 INC BX
        {Mnemonic::INC, Encoding::Reg16},        // 44 INC SP
        {Mnemonic::INC, Encoding::Reg16},        // 45 INC BP
        {Mnemonic::INC, Encoding::Reg16},        // 46 INC SI
        {Mnemonic::INC, Encoding::Reg16},        // 47 INC DI
        {Mnemonic::DEC, Encoding::Reg16},        // 48 DEC AX
        {Mnemonic::DEC, Encoding::Reg16},        // 49 DEC CX
        {Mnemonic::DEC, Encoding::Reg16},        // 4A DEC DX
        {Mnemonic::DEC, Encoding::Reg16},        // 4B DEC BX
        {Mnemonic::DEC, Encoding::Reg16},        // 4C DEC SP
        {Mnemonic::DEC, Encoding::Reg16},        // 4D DEC BP
        {Mnemonic::DEC, Encoding::Reg16},        // 4E DEC SI
        {Mnemonic::DEC, Encoding::Reg16},        // 4F DEC DI
        {Mnemonic::PUSH, Encoding::Reg16},       // 50 PUSH AX
        {Mnemonic::PUSH, Encoding::Reg16},       // 51 PUSH CX
        {Mnemonic::PUSH, Encoding::Reg16},       // 52 PUSH DX
        {Mnemonic::PUSH, Encoding::Reg16},       // 53 PUSH BX
        {Mnemonic::PUSH, Encoding::Reg16},       // 54 PUSH SP
        {Mnemonic::PUSH, Encoding::Reg16},       // 55 PUSH BP
        {Mnemonic::PUSH, Encoding::Reg16},       // 56 PUSH SI
        {Mnemonic::PUSH, Encoding::Reg16},       // 57 PUSH DI
        {Mnemonic::POP, Encoding::Reg16},        // 58 POP AX
        {Mnemonic::POP, Encoding::Reg16},        // 59 POP CX
        {Mnemonic::POP, Encoding::Reg16},        // 5A POP DX
        {Mnemonic::POP, Encoding::Reg16},        // 5B POP BX
        {Mnemonic::POP, Encoding::Reg16},        // 5C POP SP
        {Mnemonic::POP, Encoding::Reg16},        // 5D POP BP
        {Mnemonic::POP, Encoding::Reg16},        // 5E POP SI
        {Mnemonic::POP, Encoding::Reg16},        // 5F POP DI
        {Mnemonic::JO, Encoding::Rel8},          // 60 alias of 70 (undocumented)
        {Mnemonic::JNO, Encoding::Rel8},         // 61 alias of 71
        {Mnemonic::JB, Encoding::Rel8},          // 62 alias of 72
        {Mnemonic::JNB, Encoding::Rel8},         // 63 alias of 73
        {Mnemonic::JE, Encoding::Rel8},          // 64 alias of 74
        {Mnemonic::JNE, Encoding::Rel8},         // 65 alias of 75
        {Mnemonic::JBE, Encoding::Rel8},         // 66 alias of 76
        {Mnemonic::JNBE, Encoding::Rel8},        // 67 alias of 77
        {Mnemonic::JS, Encoding::Rel8},          // 68 alias of 78
        {Mnemonic::JNS, Encoding::Rel8},         // 69 alias of 79
        {Mnemonic::JP, Encoding::Rel8},          // 6A alias of 7A
        {Mnemonic::JNP, Encoding::Rel8},         // 6B alias of 7B
        {Mnemonic::JL, Encoding::Rel8},          // 6C alias of 7C
        {Mnemonic::JNL, Encoding::Rel8},         // 6D alias of 7D
        {Mnemonic::JLE, Encoding::Rel8},         // 6E alias of 7E
        {Mnemonic::JNLE, Encoding::Rel8},        // 6F alias of 7F
        {Mnemonic::JO, Encoding::Rel8},          // 70
        {Mnemonic::JNO, Encoding::Rel8},         // 71
        {Mnemonic::JB, Encoding::Rel8},          // 72
        {Mnemonic::JNB, Encoding::Rel8},         // 73
        {Mnemonic::JE, Encoding::Rel8},          // 74
        {Mnemonic::JNE, Encoding::Rel8},         // 75
        {Mnemonic::JBE, Encoding::Rel8},         // 76
        {Mnemonic::JNBE, Encoding::Rel8},        // 77
        {Mnemonic::JS, Encoding::Rel8},          // 78
        {Mnemonic::JNS, Encoding::Rel8},         // 79
        {Mnemonic::JP, Encoding::Rel8},          // 7A
        {Mnemonic::JNP, Encoding::Rel8},         // 7B
        {Mnemonic::JL, Encoding::Rel8},          // 7C
        {Mnemonic::JNL, Encoding::Rel8},         // 7D
        {Mnemonic::JLE, Encoding::Rel8},         // 7E
        {Mnemonic::JNLE, Encoding::Rel8},        // 7F
        {Mnemonic::Immed, Encoding::RmImmSExt},  // 80 r/m8, imm8
        {Mnemonic::Immed, Encoding::RmImmSExt},  // 81 r/m16, imm16
        {Mnemonic::Immed, Encoding::RmImmSExt},  // 82 alias of 80
        {Mnemonic::Immed, Encoding::RmImmSExt},  // 83 r/m16, imm8 sign-extended
        {Mnemonic::TEST, Encoding::RmReg},       // 84
        {Mnemonic::TEST, Encoding::RmReg},       // 85
        {Mnemonic::XCHG, Encoding::RmReg},       // 86
        {Mnemonic::XCHG, Encoding::RmReg},       // 87
        {Mnemonic::MOV, Encoding::RegRm},        // 88
        {Mnemonic::MOV, Encoding::RegRm},        // 89
        {Mnemonic::MOV, Encoding::RegRm},        // 8A
        {Mnemonic::MOV, Encoding::RegRm},        // 8B
        {Mnemonic::MOV, Encoding::SRegRm},       // 8C MOV r/m16, sreg
        {Mnemonic::LEA, Encoding::RegMem},       // 8D
        {Mnemonic::MOV, Encoding::SRegRm},       // 8E MOV sreg, r/m16
        {Mnemonic::POP, Encoding::Rm},           // 8F POP r/m16 (reg must be 000)
        {Mnemonic::XCHG, Encoding::AccReg16},    // 90 NOP (= XCHG AX, AX)
        {Mnemonic::XCHG, Encoding::AccReg16},    // 91
        {Mnemonic::XCHG, Encoding::AccReg16},    // 92
        {Mnemonic::XCHG, Encoding::AccReg16},    // 93
        {Mnemonic::XCHG, Encoding::AccReg16},    // 94
        {Mnemonic::XCHG, Encoding::AccReg16},    // 95
        {Mnemonic::XCHG, Encoding::AccReg16},    // 96
        {Mnemonic::XCHG, Encoding::AccReg16},    // 97
        {Mnemonic::CBW, Encoding::None},         // 98
        {Mnemonic::CWD, Encoding::None},         // 99
        {Mnemonic::CALL, Encoding::Far},         // 9A CALL far
        {Mnemonic::WAIT, Encoding::None},        // 9B
        {Mnemonic::PUSHF, Encoding::None},       // 9C
        {Mnemonic::POPF, Encoding::None},        // 9D
        {Mnemonic::SAHF, Encoding::None},        // 9E
        {Mnemonic::LAHF, Encoding::None},        // 9F
        {Mnemonic::MOV, Encoding::AccMem},       // A0 MOV AL, [addr]
        {Mnemonic::MOV, Encoding::AccMem},       // A1 MOV AX, [addr]
        {Mnemonic::MOV, Encoding::AccMem},       // A2 MOV [addr], AL
        {Mnemonic::MOV, Encoding::AccMem},       // A3 MOV [addr], AX
        {Mnemonic::MOVS, Encoding::String},      // A4
        {Mnemonic::MOVS, Encoding::String},      // A5
        {Mnemonic::CMPS, Encoding::String},      // A6
        {Mnemonic::CMPS, Encoding::String},      // A7
        {Mnemonic::TEST, Encoding::AccImm},      // A8
        {Mnemonic::TEST, Encoding::AccImm},      // A9
        {Mnemonic::STOS, Encoding::String},      // AA
        {Mnemonic::STOS, Encoding::String},      // AB
        {Mnemonic::LODS, Encoding::String},      // AC
        {Mnemonic::LODS, Encoding::String},      // AD
        {Mnemonic::SCAS, Encoding::String},      // AE
        {Mnemonic::SCAS, Encoding::String},      // AF
        {Mnemonic::MOV, Encoding::RegImm},       // B0 MOV AL, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B1 MOV CL, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B2 MOV DL, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B3 MOV BL, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B4 MOV AH, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B5 MOV CH, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B6 MOV DH, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B7 MOV BH, imm8
        {Mnemonic::MOV, Encoding::RegImm},       // B8 MOV AX, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // B9 MOV CX, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BA MOV DX, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BB MOV BX, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BC MOV SP, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BD MOV BP, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BE MOV SI, imm16
        {Mnemonic::MOV, Encoding::RegImm},       // BF MOV DI, imm16
        {Mnemonic::RET, Encoding::Imm16},        // C0 alias of C2 (undocumented)
        {Mnemonic::RET, Encoding::None},         // C1 alias of C3 (undocumented)
        {Mnemonic::RET, Encoding::Imm16},        // C2 RET imm16
        {Mnemonic::RET, Encoding::None},         // C3 RET
        {Mnemonic::LES, Encoding::RegMem},       // C4
        {Mnemonic::LDS, Encoding::RegMem},       // C5
        {Mnemonic::MOV, Encoding::RmImm},        // C6 MOV r/m8, imm8 (reg must be 000)
        {Mnemonic::MOV, Encoding::RmImm},        // C7 MOV r/m16, imm16 (reg must be 000)
        {Mnemonic::RET, Encoding::Imm16},        // C8 alias of CA (undocumented, far)
        {Mnemonic::RET, Encoding::None},         // C9 alias of CB (undocumented, far)
        {Mnemonic::RET, Encoding::Imm16},        // CA RETF imm16
        {Mnemonic::RET, Encoding::None},         // CB RETF
        {Mnemonic::INT, Encoding::None},         // CC INT 3
        {Mnemonic::INT, Encoding::Imm8},         // CD INT imm8
        {Mnemonic::INTO, Encoding::None},        // CE
        {Mnemonic::IRET, Encoding::None},        // CF
        {Mnemonic::Shift, Encoding::RmShift},    // D0 r/m8, 1
        {Mnemonic::Shift, Encoding::RmShift},    // D1 r/m16, 1
        {Mnemonic::Shift, Encoding::RmShift},    // D2 r/m8, CL
        {Mnemonic::Shift, Encoding::RmShift},    // D3 r/m16, CL
        {Mnemonic::AAM, Encoding::Imm8},         // D4 (imm8 = base, normally 0x0A)
        {Mnemonic::AAD, Encoding::Imm8},         // D5 (imm8 = base, normally 0x0A)
        {Mnemonic::NONE, Encoding::None},        // D6 SALC (undocumented)
        {Mnemonic::XLAT, Encoding::None},        // D7
        {Mnemonic::ESC, Encoding::Esc},          // D8
        {Mnemonic::ESC, Encoding::Esc},          // D9
        {Mnemonic::ESC, Encoding::Esc},          // DA
        {Mnemonic::ESC, Encoding::Esc},          // DB
        {Mnemonic::ESC, Encoding::Esc},          // DC
        {Mnemonic::ESC, Encoding::Esc},          // DD
        {Mnemonic::ESC, Encoding::Esc},          // DE
        {Mnemonic::ESC, Encoding::Esc},          // DF
        {Mnemonic::LOOPNZ, Encoding::Rel8},      // E0
        {Mnemonic::LOOPZ, Encoding::Rel8},       // E1
        {Mnemonic::LOOP, Encoding::Rel8},        // E2
        {Mnemonic::JCXZ, Encoding::Rel8},        // E3
        {Mnemonic::IN, Encoding::AccPort},       // E4 IN AL, imm8
        {Mnemonic::IN, Encoding::AccPort},       // E5 IN AX, imm8
        {Mnemonic::OUT, Encoding::AccPort},      // E6 OUT imm8, AL
        {Mnemonic::OUT, Encoding::AccPort},      // E7 OUT imm8, AX
        {Mnemonic::CALL, Encoding::Rel16},       // E8 CALL near
        {Mnemonic::JMP, Encoding::Rel16},        // E9 JMP near
        {Mnemonic::JMP, Encoding::Far},          // EA JMP far
        {Mnemonic::JMP, Encoding::Rel8},         // EB JMP short
        {Mnemonic::IN, Encoding::AccDx},         // EC IN AL, DX
        {Mnemonic::IN, Encoding::AccDx},         // ED IN AX, DX
        {Mnemonic::OUT, Encoding::AccDx},        // EE OUT DX, AL
        {Mnemonic::OUT, Encoding::AccDx},        // EF OUT DX, AX
        {Mnemonic::LOCK, Encoding::Prefix},      // F0
        {Mnemonic::LOCK, Encoding::Prefix},      // F1 alias of F0 (undocumented)
        {Mnemonic::REP, Encoding::Prefix},       // F2 REPNE/REPNZ
        {Mnemonic::REP, Encoding::Prefix},       // F3 REP/REPE/REPZ
        {Mnemonic::HLT, Encoding::None},         // F4
        {Mnemonic::CMC, Encoding::None},         // F5
        {Mnemonic::Grp1, Encoding::Group3},      // F6 r/m8
        {Mnemonic::Grp1, Encoding::Group3},      // F7 r/m16
        {Mnemonic::CLC, Encoding::None},         // F8
        {Mnemonic::STC, Encoding::None},         // F9
        {Mnemonic::CLI, Encoding::None},         // FA
        {Mnemonic::STI, Encoding::None},         // FB
        {Mnemonic::CLD, Encoding::None},         // FC
        {Mnemonic::STD, Encoding::None},         // FD
        {Mnemonic::Grp2, Encoding::Rm},          // FE r/m8 (only INC/DEC valid)
        {Mnemonic::Grp2, Encoding::Rm},          // FF r/m16
    };

    // deduce general instruction type
    u8 op = instr.advance();
    instr.mnemonic = OPCODES[op].mnemonic;
    instr.encoding = OPCODES[op].encoding;

    instr.d = op & DIR_MASK;
    instr.w = op & WORD_MASK;


    // set mod reg rm fields even if its not such an instr.
    u8 byte2 = instr.peek();
    instr.mod = (byte2 & MOD_MASK) >> 6;
    instr.reg = (byte2 & REG_MASK) >> 3;
    instr.rm = (byte2 & RM_MASK);

    // finalize instruction type
    // 80-83: operation from reg field
    if (instr.mnemonic == Mnemonic::Immed) {
        OpcodeInfo immType[8] = {
            {Mnemonic::ADD, Encoding::RmImmSExt},   // 000
            {Mnemonic::OR, Encoding::RmImmSExt},    // 001
            {Mnemonic::ADC, Encoding::RmImmSExt},   // 010
            {Mnemonic::SBB, Encoding::RmImmSExt},   // 011
            {Mnemonic::AND, Encoding::RmImmSExt},   // 100
            {Mnemonic::SUB, Encoding::RmImmSExt},   // 101
            {Mnemonic::XOR, Encoding::RmImmSExt},   // 110
            {Mnemonic::CMP, Encoding::RmImmSExt},   // 111
        };
        instr.mnemonic = immType[instr.reg].mnemonic;
        instr.encoding = immType[instr.reg].encoding;
    }

    // D0-D3: operation from reg field
    else if (instr.mnemonic == Mnemonic::Shift) {
        OpcodeInfo shiftType[8] = {
            {Mnemonic::ROL, Encoding::RmShift},     // 000
            {Mnemonic::ROR, Encoding::RmShift},     // 001
            {Mnemonic::RCL, Encoding::RmShift},     // 010
            {Mnemonic::RCR, Encoding::RmShift},     // 011
            {Mnemonic::SHL, Encoding::RmShift},     // 100
            {Mnemonic::SHR, Encoding::RmShift},     // 101
            {Mnemonic::NONE, Encoding::RmShift},    // 110 SETMO (undocumented)
            {Mnemonic::SAR, Encoding::RmShift},     // 111
        };
        instr.mnemonic = shiftType[instr.reg].mnemonic;
        instr.encoding = shiftType[instr.reg].encoding;
    }

    // F6/F7: operation from reg field
    else if (instr.mnemonic == Mnemonic::Grp1) {
        OpcodeInfo grp1Type[8] = {
            {Mnemonic::TEST, Encoding::Group3},     // 000 TEST r/m, imm
            {Mnemonic::TEST, Encoding::Group3},     // 001 alias of 000 (undocumented)
            {Mnemonic::NOT, Encoding::Group3},      // 010
            {Mnemonic::NEG, Encoding::Group3},      // 011
            {Mnemonic::MUL, Encoding::Group3},      // 100
            {Mnemonic::IMUL, Encoding::Group3},     // 101
            {Mnemonic::DIV, Encoding::Group3},      // 110
            {Mnemonic::IDIV, Encoding::Group3},     // 111
        };
        instr.mnemonic = grp1Type[instr.reg].mnemonic;
        instr.encoding = grp1Type[instr.reg].encoding;
    }

    // FE/FF: operation from reg field
    else if (instr.mnemonic == Mnemonic::Grp2) {
        OpcodeInfo grp2Type[8] = {
            {Mnemonic::INC, Encoding::Rm},          // 000
            {Mnemonic::DEC, Encoding::Rm},          // 001
            {Mnemonic::CALL, Encoding::Rm},         // 010 CALL near indirect (FF only)
            {Mnemonic::CALL, Encoding::Rm},         // 011 CALL far indirect (FF only)
            {Mnemonic::JMP, Encoding::Rm},          // 100 JMP near indirect (FF only)
            {Mnemonic::JMP, Encoding::Rm},          // 101 JMP far indirect (FF only)
            {Mnemonic::PUSH, Encoding::Rm},         // 110 PUSH r/m16 (FF only)
            {Mnemonic::NONE, Encoding::Rm},         // 111 undefined
        };
        instr.mnemonic = grp2Type[instr.reg].mnemonic;
        instr.encoding = grp2Type[instr.reg].encoding;
    }

    bool hasModRegRm = instr.encoding >= Encoding::RegRm;

    Operand regOp;
    Operand rmOp;

    if ( instr.encoding == Encoding::SRegRm || instr.encoding == Encoding::RegMem ) {
        instr.w = true;
    }

    if (hasModRegRm) {
        instr.advance(); // skip modregrm byte
        regOp = makeReg(instr.reg, instr.w);
        rmOp = decodeRm(instr);
        if (rmOp.kind == OpKind::MEM) {
            rmOp.segOverride = segOverrideFromPrefixes(instr);
        }
    }

    switch (instr.encoding) {
        default: break;

        case Encoding::None:
        case Encoding::Prefix:
        case Encoding::String: break;

        case Encoding::SReg:
            instr.dst = {.kind = OpKind::REG, .reg = solveSegment((op >> 3) & 0x3)};
            break;

        case Encoding::Reg16:
            instr.dst = {.kind = OpKind::REG, .reg = solveRegister((op & 0x7), true)};
            break;

        case Encoding::AccReg16:
            instr.dst = {.kind = OpKind::REG, .reg = Register::AX};
            instr.src = {.kind = OpKind::REG, .reg = solveRegister((op & 0x7), true)};
            break; // do nothing dont fetch bytes

        case Encoding::RegImm:
            instr.w = (op >> 3) & 0x1;
            instr.dst = {.kind = OpKind::REG, .reg = solveRegister((op & 0x7), instr.w)};
            instr.src = {.kind = OpKind::IMM, .imm = solveImm(instr, instr.w)};
            break;

        case Encoding::AccImm:
            instr.dst = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
            instr.src = {.kind = OpKind::IMM, .imm = solveImm(instr, instr.w)};
            break;

        case Encoding::AccMem: {

            Operand acc = instr.dst = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
            Operand mem = {.kind = OpKind::MEM, .base = eaBase::Direct, .disp = (i16)solveImm(instr, true)};

            mem.segOverride = segOverrideFromPrefixes(instr);

            if (!(op & 0x2)) {
                instr.dst = acc;
                instr.src = mem;
            } else {
                instr.dst = mem;
                instr.src = acc;
            }

            break;
        }

        case Encoding::AccPort:
            if (!(op & 0x2)) {
                instr.dst = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
                instr.src = {.kind = OpKind::MEM, .base = eaBase::Direct, .disp = (i8)solveImm(instr, false)};
            } else {
                instr.dst = {.kind = OpKind::MEM, .base = eaBase::Direct, .disp = (i8)solveImm(instr, false)};
                instr.src = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
            }
            break;

        case Encoding::AccDx:
            if (!(op & 0x2)) {
                instr.dst = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
                instr.src = {.kind = OpKind::REG, .reg = solveRegister((0b010), true)};
            } else {
                instr.dst = {.kind = OpKind::REG, .reg = solveRegister((0b010), true)};
                instr.src = {.kind = OpKind::REG, .reg = solveRegister((0b000), instr.w)};
            }
            break;

        case Encoding::Imm8:
            instr.dst = {.kind = OpKind::IMM, .imm = solveImm(instr, false)};
            break;

        case Encoding::Imm16:
            instr.dst = {.kind = OpKind::IMM, .imm = solveImm(instr, true)};
            break;

        case Encoding::Rel8:
            instr.dst = {.kind = OpKind::REL, .disp = (i8)solveImm(instr, false)};
            break;

        case Encoding::Rel16:
            instr.dst = {.kind = OpKind::REL, .disp = (i16)solveImm(instr, true)};
            break;

        case Encoding::Far: {
            u16 offset16  = instr.advance16();
            u16 segment16 = instr.advance16();
            instr.dst = {.kind = OpKind::FAR, .disp = static_cast<i16>(offset16), .imm = segment16};
            break;
        }



        case Encoding::RegRm: {

            if (instr.d) {
                instr.dst = regOp;
                instr.src = rmOp;
            } else {
                instr.dst = rmOp;
                instr.src = regOp;
            }
            break;
        }

        case Encoding::RmReg:
            instr.dst = rmOp;
            instr.src = regOp;
            break;

        case Encoding::SRegRm:

            regOp = {.kind = OpKind::REG, .reg = solveSegment(instr.reg & 0x3)};

            if (instr.d) {
                instr.dst = regOp;
                instr.src = rmOp;
            }

            else {
                instr.dst = rmOp;
                instr.src = regOp;
            }

            break;

        case Encoding::RegMem:
            instr.dst = regOp;
            instr.src = rmOp;
            break;

        case Encoding::RmImm:
            instr.dst = rmOp;
            instr.src = {.kind = OpKind::IMM, .imm = solveImm(instr, instr.w)};
            break;

        case Encoding::RmImmSExt: {
            instr.dst = rmOp;
            u16 imm = solveImm(instr, op == 0x81);
            if (op == 0x83)
                imm = (u16)(i16)(i8)(imm); // sign extend
            instr.src = {.kind = OpKind::IMM, .imm = imm};
            break;
        }

        case Encoding::RmShift:
            instr.dst = rmOp;
            if (op & 0x2) {
                instr.src = {.kind = OpKind::REG, .reg = solveRegister((0b001), false)};
            } else {
                instr.src = {.kind = OpKind::IMM, .imm = 1};
            }
            break;

        case Encoding::Group3:
            instr.dst = rmOp;
            if ( instr.reg == 0 || instr.reg == 1 ) {
                instr.src = {.kind = OpKind::IMM, .imm = solveImm(instr, instr.w)};
            }
            break;

        case Encoding::Rm:
            instr.dst = rmOp;
            break;

        case Encoding::Esc:
            break;
            
        
    }


    instr.length = instr.position;
    return instr;
}