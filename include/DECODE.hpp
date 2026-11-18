#pragma once
#include <cstdint>
#include <vector>

#define MOD_MASK 0xC0
#define REG_MASK 0x38
#define RM_MASK 0x07

#define DIR_MASK 0x02
#define WORD_MASK 0x01

enum class PREFIX : uint8_t {
    NONE   = 0x00,
    REP    = 0xF3,
    REPNE  = 0xF2,
    CS_OVR = 0x2E,
    DS_OVR = 0x3E,
    ES_OVR = 0x26,
    SS_OVR = 0x36,
    LOCK   = 0xF0
};

enum class Register : uint8_t {
    NONE,

    AX, BX, CX, DX,
    SP, BP, SI, DI,

    AL, BL, CL, DL,
    AH, BH, CH, DH,

    ES, CS, SS, DS
};


enum class eaBase : uint8_t {
    BX_SI, BX_DI, BP_SI, BP_DI, SI, DI, BP, BX,
    Direct
};

enum class Mnemonic {
    NONE,
    ADD, PUSH, POP, OR,
    ADC, SBB,
    AND, SEG, DAA, SUB, DAS,
    XOR, AAA, CMP, AAS,
    INC, DEC,
    JO, JNO, JB, JNB, JE, JNE, JBE, JNBE, JS, JNS, JP, JNP, JL, JNL, JLE, JNLE,
    Immed, TEST, XCHG, MOV, LEA,
    CBW, CWD, CALL, WAIT, PUSHF, POPF, SAHF, LAHF,
    MOVS, CMPS, STOS, LODS, SCAS,
    RET, LES, LDS, INT, INTO, IRET,
    Shift, AAM, AAD, XLAT, ESC,
    LOOPNZ, LOOPZ, LOOP, JCXZ, IN, OUT, JMP,
    LOCK, REP, HLT, CMC, Grp1, CLC, STC, CLI, STI, CLD, STD, Grp2,

    // // shift group types
    ROL,
    ROR,
    RCL,
    RCR,
    SHL,
    SHR,
    SAR,

    // grp1 types
    NOT,
    NEG,
    MUL,
    IMUL,
    DIV,
    IDIV,
};

enum class Encoding : uint8_t {
    // no ModR/M
    None,        // opcode only, no operands
    Prefix,      // segment override, LOCK, REP/REPNE
    String,      // MOVS/CMPS/STOS/LODS/SCAS, implicit SI/DI
    SReg,        // segment register in opcode bits 4-3 (PUSH/POP sreg)
    Reg16,       // 16-bit register in opcode bits 2-0 (INC/DEC/PUSH/POP)
    AccReg16,    // XCHG AX, reg16 (reg in bits 2-0)
    RegImm,      // MOV reg, imm (reg in bits 2-0, W in bit 3)
    AccImm,      // AL/AX, imm8/imm16
    AccMem,      // AL/AX <-> [addr16]
    AccPort,     // IN/OUT AL/AX, imm8 port
    AccDx,       // IN/OUT AL/AX, DX
    Imm8,        // INT n, AAM, AAD
    Imm16,       // RET/RETF imm16
    Rel8,        // short jumps, Jcc, LOOP, JCXZ
    Rel16,       // near CALL/JMP
    Far,         // far CALL/JMP, offset16 + segment16

    // with ModR/M (keep below this line)
    RegRm,       // reg <-> r/m with D and W bit (general case)
    RmReg,       // r/m, reg without D bit (TEST, XCHG)
    SRegRm,      // MOV sreg <-> r/m
    RegMem,      // reg <- mem, 16-bit (LEA, LES, LDS)
    RmImm,       // MOV r/m, imm (size from W)
    RmImmSExt,   // group 1: r/m, imm (size from S and W)
    RmShift,     // group 2: shift/rotate by 1 or CL
    Group3,      // TEST/NOT/NEG/MUL/IMUL/DIV/IDIV r/m (imm only for TEST)
    Rm,          // single r/m operand (POP r/m, group 4/5)
    Esc,         // 8087 escape, ModR/M only
};

struct OpcodeInfo {
    Mnemonic mnemonic;
    Encoding encoding;
};

enum class OpKind : uint8_t {
    NONE, REG, MEM, IMM, REL, FAR
};

struct Operand {
    OpKind kind = OpKind::NONE;

    // register including segment regs
    Register reg = Register::NONE;

    // Mem
    eaBase base = eaBase::Direct;
    int16_t disp = 0;
    Register segOverride = Register::NONE;

    // Imm
    uint16_t imm = 0;
};



struct Instruction {

    const uint8_t* bytes;
    uint16_t addr;
    uint8_t position;

    // in the order they came in
    std::vector<PREFIX> prefixes;

    // general instr type
    Mnemonic mnemonic;
    Encoding encoding;

    bool d;
    bool w;

    // byte 2 fields
    uint8_t mod;
    uint8_t reg;
    uint8_t rm;

    // operands
    Operand src;
    Operand dst;


    uint8_t length;
    uint8_t advance() {
        if ((position + 1) >= 16)
            return 0;
        return bytes[position++];
    }

    uint16_t advance16() {
        if (position + 2 >= 16)
            return 0;

        uint8_t lo = advance();
        uint8_t hi = advance();
        return (uint16_t)(lo | (hi << 8));
    }

    uint8_t peek() {
        if ((position) >= 16)
            return 0;
        return bytes[position];
    }


};


Instruction decode(const uint8_t* buf, uint16_t addr);