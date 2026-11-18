#pragma once
#include <format>

#include "DECODE.hpp"
#include "DEFS.hpp"

inline std::string strPrefix(PREFIX p) {
    switch (p) {
        case PREFIX::NONE: return "none";
        case PREFIX::REP: return "rep";
        case PREFIX::REPNE: return "repne";
        case PREFIX::CS_OVR: return ""; // CS override
        case PREFIX::DS_OVR: return ""; // DS override
        case PREFIX::ES_OVR: return ""; // ES override
        case PREFIX::SS_OVR: return ""; // SS override
        case PREFIX::LOCK: return "lock";
    }

    return "???";
}

inline std::string strMnemonic(Mnemonic type, bool w) {
    switch (type) {


        case Mnemonic::NONE: return "none";
        case Mnemonic::ADD: return "add";
        case Mnemonic::PUSH: return "push";
        case Mnemonic::POP: return "pop";
        case Mnemonic::OR: return "or";

        case Mnemonic::ADC: return "adc";
        case Mnemonic::SBB: return "sbb";

        case Mnemonic::AND: return "and";
        case Mnemonic::SEG: return "seg";
        case Mnemonic::DAA: return "daa";
        case Mnemonic::SUB: return "sub";
        case Mnemonic::DAS: return "das";

        case Mnemonic::XOR: return "xor";
        case Mnemonic::AAA: return "aaa";
        case Mnemonic::CMP: return "cmp";
        case Mnemonic::AAS: return "aas";

        case Mnemonic::INC: return "inc";
        case Mnemonic::DEC: return "dec";

        case Mnemonic::JO: return "jo";
        case Mnemonic::JNO: return "jno";
        case Mnemonic::JB: return "jb";
        case Mnemonic::JNB: return "jnb";
        case Mnemonic::JE: return "je";
        case Mnemonic::JNE: return "jne";
        case Mnemonic::JBE: return "jbe";
        case Mnemonic::JNBE: return "jnbe";
        case Mnemonic::JS: return "js";
        case Mnemonic::JNS: return "jns";
        case Mnemonic::JP: return "jp";
        case Mnemonic::JNP: return "jnp";
        case Mnemonic::JL: return "jl";
        case Mnemonic::JNL: return "jnl";
        case Mnemonic::JLE: return "jle";
        case Mnemonic::JNLE: return "jnle";

        case Mnemonic::Immed: return "immed";
        case Mnemonic::TEST: return "test";
        case Mnemonic::XCHG: return "xchg";
        case Mnemonic::MOV: return "mov";
        case Mnemonic::LEA: return "lea";

        case Mnemonic::CBW: return "cbw";
        case Mnemonic::CWD: return "cwd";
        case Mnemonic::CALL: return "call";
        case Mnemonic::WAIT: return "wait";
        case Mnemonic::PUSHF: return "pushf";
        case Mnemonic::POPF: return "popf";
        case Mnemonic::SAHF: return "sahf";
        case Mnemonic::LAHF: return "lahf";

        case Mnemonic::MOVS: return w ? "movsw" : "movsb";
        case Mnemonic::CMPS: return w ? "cmpsw" : "cmpsb";
        case Mnemonic::STOS: return w ? "stosw" : "stosb";
        case Mnemonic::LODS: return w ? "lodsw" : "lodsb";
        case Mnemonic::SCAS: return w ? "scasw" : "scasb";

        case Mnemonic::RET: return "ret";
        case Mnemonic::LES: return "les";
        case Mnemonic::LDS: return "lds";
        case Mnemonic::INT: return "int";
        case Mnemonic::INTO: return "into";
        case Mnemonic::IRET: return "iret";

        case Mnemonic::Shift: return "shift";
        case Mnemonic::AAM: return "aam";
        case Mnemonic::AAD: return "aad";
        case Mnemonic::XLAT: return "xlat";
        case Mnemonic::ESC: return "esc";

        case Mnemonic::LOOPNZ: return "loopnz";
        case Mnemonic::LOOPZ: return "loopz";
        case Mnemonic::LOOP: return "loop";
        case Mnemonic::JCXZ: return "jcxz";
        case Mnemonic::IN: return "in";
        case Mnemonic::OUT: return "out";
        case Mnemonic::JMP: return "jmp";

        case Mnemonic::LOCK: return "lock";
        case Mnemonic::REP: return "rep";
        case Mnemonic::HLT: return "hlt";
        case Mnemonic::CMC: return "cmc";
        case Mnemonic::Grp1: return "grp1";
        case Mnemonic::CLC: return "clc";
        case Mnemonic::STC: return "stc";
        case Mnemonic::CLI: return "cli";
        case Mnemonic::STI: return "sti";
        case Mnemonic::CLD: return "cld";
        case Mnemonic::STD: return "std";
        case Mnemonic::Grp2: return "grp2";

        case Mnemonic::ROL: return "rol";
        case Mnemonic::ROR: return "ror";
        case Mnemonic::RCL: return "rcl";
        case Mnemonic::RCR: return "rcr";
        case Mnemonic::SHL: return "shl";
        case Mnemonic::SHR: return "shr";
        case Mnemonic::SAR: return "sar";

        case Mnemonic::NOT: return "not";
        case Mnemonic::NEG: return "neg";
        case Mnemonic::MUL: return "mul";
        case Mnemonic::IMUL: return "imul";
        case Mnemonic::DIV: return "div";
        case Mnemonic::IDIV: return "idiv";
    }

}

inline std::string strEaBase(eaBase base) {
    switch (base) {
        case eaBase::BX_SI: return "bx + si";
        case eaBase::BX_DI: return "bx + di";
        case eaBase::BP_SI: return "bp + si";
        case eaBase::BP_DI: return "bp + di";
        case eaBase::SI: return "si";
        case eaBase::DI: return "di";
        case eaBase::BP: return "bp";
        case eaBase::BX: return "bx";
        case eaBase::Direct: return "direct";
    }
}

inline std::string strRegister(Register reg) {
    switch (reg) {
        case Register::NONE: return "none";
        case Register::AX: return "ax";
        case Register::BX: return "bx";
        case Register::CX: return "cx";
        case Register::DX: return "dx";
        case Register::SP: return "sp";
        case Register::BP: return "bp";
        case Register::SI: return "si";
        case Register::DI: return "di";
        case Register::AL: return "al";
        case Register::BL: return "bl";
        case Register::CL: return "cl";
        case Register::DL: return "dl";
        case Register::AH: return "ah";
        case Register::BH: return "bh";
        case Register::CH: return "ch";
        case Register::DH: return "dh";

        case Register::ES: return "es";
        case Register::CS: return "cs";
        case Register::SS: return "ss";
        case Register::DS: return "ds";
    }
}

inline std::string strOperand(Operand op) {
    switch (op.kind) {
        case OpKind::REG:
            return strRegister(op.reg);

        case OpKind::IMM:
            return std::to_string(op.imm);

        case OpKind::MEM: {
            std::string s;

            if (op.segOverride != Register::NONE) {
                s += strRegister(op.segOverride) + ":";
            }

            if (op.base == eaBase::Direct) {
                return s + "[" + std::format("0x{:04X}", (u16)(op.disp)) + "]";
            }

            s += "[" + strEaBase(op.base);
            if (op.disp > 0) s += " + " + std::to_string(op.disp);
            if (op.disp < 0) s += " - " + std::to_string(-(int)op.disp);
            return s + "]";
        }

        default:
            return "";
        
    }
}

inline std::string strOperandDebug(Operand op) {
    std::string str;

    str += "Kind: ";
    switch (op.kind) {
        case OpKind::NONE: str += "NONE "; break;
        case OpKind::REG: str += "REG "; break;
        case OpKind::MEM: str += "MEM "; break;
        case OpKind::IMM: str += "IMM "; break;
        case OpKind::REL: str += "REL "; break;
        case OpKind::FAR: str += "FAR "; break;
    }
    str += "\n";

    str += "Register: ";
    str += strRegister(op.reg);
    str += "\n";

    str += "Base: ";
    str += strEaBase(op.base);
    str += "\n";

    str += "disp: ";
    str += std::format("{:04X}\n", op.disp);

    str += "seg override: ";
    str += strRegister(op.segOverride);
    str += "\n";

    str += "imm: ";
    str += std::format("{:04X}\n", op.imm);


    return str;
}


inline std::string strEncoding(Encoding enc) {
    switch (enc) {

        case Encoding::None: return "None";
        case Encoding::Prefix: return "Prefix";
        case Encoding::String: return "String";
        case Encoding::SReg: return "SReg";
        case Encoding::Reg16: return "Reg16";
        case Encoding::AccReg16: return "AccReg16";
        case Encoding::RegImm: return "RegImm";
        case Encoding::AccImm: return "AccImm";
        case Encoding::AccMem: return "AccMem";
        case Encoding::AccPort: return "AccPort";
        case Encoding::AccDx: return "AccDx";
        case Encoding::Imm8: return "Imm8";
        case Encoding::Imm16: return "Imm16";
        case Encoding::Rel8: return "Rel8";
        case Encoding::Rel16: return "Rel16";
        case Encoding::Far: return "Far";
        case Encoding::RegRm: return "RegRm";
        case Encoding::RmReg: return "RmReg";
        case Encoding::SRegRm: return "SRegRm";
        case Encoding::RegMem: return "RegMem";
        case Encoding::RmImm: return "RmImm";
        case Encoding::RmImmSExt: return "RmImmSExt";
        case Encoding::RmShift: return "RmShift";
        case Encoding::Group3: return "Group3";
        case Encoding::Rm: return "Rm";
        case Encoding::Esc: return "Esc";
    }

}

inline std::string strInstruction(const Instruction& instr) {
    std::string s = "";




    for ( int i = 0; i < instr.prefixes.size(); i++ ) {
        std::string p = strPrefix( instr.prefixes.at(i) );
        s += p;
        if (!p.empty()) s += " ";
    }

    if ( instr.mnemonic == Mnemonic::XCHG &&
    instr.dst.kind == OpKind::REG && instr.dst.reg == Register::AX &&
    instr.src.kind == OpKind::REG && instr.src.reg == Register::AX )
        return "nop";
    else
        s += strMnemonic(instr.mnemonic, instr.w);



    if (instr.dst.kind == OpKind::NONE) {
        return s;
    }

    s += " ";

    bool needSize = (instr.dst.kind == OpKind::MEM) &&
                    (instr.src.kind != OpKind::REG || instr.encoding == Encoding::RmShift);

    if (needSize) {
        if ( instr.mnemonic == Mnemonic::CALL || instr.mnemonic == Mnemonic::JMP )
            s += (instr.reg & 1) ? "far " : "";
        else
            s += instr.w ? "word " : "byte ";
    }

    s += strOperand(instr.dst);

    if (instr.src.kind != OpKind::NONE)
        s += ", " + strOperand(instr.src);

    return s;
}

inline std::string strInstructionDebug(const Instruction& instr) {
    std::string str;

    str += "bytes: \n";
    str += "\t";
    for ( int i = 0; i < 16; i++ ) {
        str += std::format("0x{:02X} ", instr.bytes[i]);
    }
    str += "\n";

    str += "length: \n";
    str += "\t";
    str += std::to_string(instr.length);
    str += "\n";

    str += "prefixes: \n";
    for ( int i = 0; i < instr.prefixes.size(); i++ ) {
        str += "\t";
        str += strPrefix( instr.prefixes.at(i) );
        str += "\n";
    }

    str += "mnemonic: \n";
    str += "\t";
    str += strMnemonic(instr.mnemonic, instr.w);
    str += "\n";

    str += "encoding: \n";
    str += "\t";
    str += strEncoding(instr.encoding);
    str += "\n";

    str += "d: ";
    str += (instr.d) ? "true" : "false";
    str += "\n";

    str += "w: ";
    str += (instr.w) ? "true" : "false";
    str += "\n";

    str += "operands: \n";
    str += "dst: ";
    str += strOperandDebug(instr.dst) + "\n";
    str += "src: ";
    str += strOperandDebug(instr.src);
    str += "\n";

    str += "line: \n";
    str += "\t";
    str += strInstruction(instr);
    str += "\n";


    return str;
}